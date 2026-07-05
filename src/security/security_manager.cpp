#include "src/security/security_manager.h"

#include "src/security/certificate_verifier.h"
#include "src/security/sandbox_policy.h"
#include "src/security/content_security_policy.h"
#include "src/common/logging.h"
#include "src/common/config.h"
#include "src/common/string_utils.h"

namespace nullware {

SecurityManager::SecurityManager() 
    : security_level_(SecurityLevel::MEDIUM),
      https_only_mode_(false),
      certificate_pinning_(true),
      csp_enforcement_(true),
      safe_browsing_(true) {
    statistics_ = {0, 0, 0, 0, 0};
}

SecurityManager::~SecurityManager() {
    Shutdown();
}

bool SecurityManager::Initialize() {
    LOG_INFO("Initializing SecurityManager");
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        // Initialize components
        cert_verifier_ = std::make_unique<CertificateVerifier>();
        sandbox_policy_ = std::make_unique<SandboxPolicy>();
        csp_manager_ = std::make_unique<ContentSecurityPolicy>();
        
        // Load default settings from config
        const auto& config = ConfigManager::GetInstance().GetConfig();
        https_only_mode_ = config.https_only_mode;
        certificate_pinning_ = config.certificate_pinning;
        csp_enforcement_ = config.csp_enforcement;
        safe_browsing_ = config.safe_browsing;
        
        // Initialize default settings
        InitializeDefaultSettings();
        
        // Load malicious URL database
        LoadMaliciousUrlDatabase();
        
        LOG_INFO("SecurityManager initialized successfully");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to initialize SecurityManager: " + std::string(e.what()));
        return false;
    }
}

void SecurityManager::Shutdown() {
    LOG_INFO("Shutting down SecurityManager");
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (cert_verifier_) {
        cert_verifier_.reset();
    }
    if (sandbox_policy_) {
        sandbox_policy_.reset();
    }
    if (csp_manager_) {
        csp_manager_.reset();
    }
    
    malicious_urls_.clear();
    pinned_certificates_.clear();
    
    LOG_INFO("SecurityManager shutdown complete");
}

SecurityCheckResult SecurityManager::CheckUrlSafety(const std::string& url, const std::string& source_url) {
    SecurityCheckResult result;
    result.is_safe = true;
    result.threat_type = ThreatType::NONE;
    result.required_level = security_level_;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    statistics_.urls_checked++;
    
    // Check for known malicious URLs
    if (IsKnownMaliciousUrl(url)) {
        result.is_safe = false;
        result.threat_type = ThreatType::MALWARE;
        result.details = "URL is in the malicious URL database";
        statistics_.threats_detected++;
        return result;
    }
    
    // Check for HTTPS only mode violations
    if (IsHttpsOnlyViolation(url)) {
        result.is_safe = false;
        result.threat_type = ThreatType::SOCIAL_ENGINEERING;
        result.details = "HTTP request blocked in HTTPS-only mode";
        statistics_.threats_detected++;
        return result;
    }
    
    // Check for mixed content
    if (ShouldBlockMixedContent(url, source_url)) {
        result.is_safe = false;
        result.threat_type = ThreatType::SOCIAL_ENGINEERING;
        result.details = "Mixed content blocked";
        statistics_.mixed_content_blocked++;
        return result;
    }
    
    // Additional security checks can be added here
    // For example: safe browsing API calls, reputation checks, etc.
    
    return result;
}

bool SecurityManager::VerifyCertificate(const std::string& hostname, const std::vector<uint8_t>& cert_data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    statistics_.certificates_verified++;
    
    if (!cert_verifier_) {
        return false;
    }
    
    // Check if we have a pinned certificate for this hostname
    if (certificate_pinning_ && HasPinnedCertificate(hostname)) {
        auto it = pinned_certificates_.find(hostname);
        if (it != pinned_certificates_.end()) {
            // Compare certificate data
            if (it->second == cert_data) {
                return true;  // Certificate matches pinned certificate
            } else {
                LOG_WARNING("Certificate pinning failed for: " + hostname);
                return false;  // Certificate doesn't match pinned certificate
            }
        }
    }
    
    // Use the certificate verifier
    return cert_verifier_->VerifyCertificate(hostname, cert_data);
}

bool SecurityManager::CheckContentSecurityPolicy(const std::string& url, const std::string& csp_header) {
    if (!csp_enforcement_ || !csp_manager_) {
        return true;  // Allow if CSP enforcement is disabled
    }
    
    return csp_manager_->CheckPolicy(url, csp_header);
}

bool SecurityManager::ShouldAllowRequest(const std::string& url, const std::string& method,
                                       const std::vector<std::pair<std::string, std::string>>& headers,
                                       const std::string& source_url) {
    // Check URL safety first
    SecurityCheckResult safety_result = CheckUrlSafety(url, source_url);
    if (!safety_result.is_safe) {
        LOG_DEBUG("Request blocked due to safety check: " + url + " - " + safety_result.details);
        return false;
    }
    
    // Check for mixed content
    if (ShouldBlockMixedContent(url, source_url)) {
        LOG_DEBUG("Request blocked due to mixed content: " + url);
        return false;
    }
    
    // Check Content Security Policy
    // (This would be called with the actual CSP header from the response)
    
    // Additional request-specific checks can be added here
    
    return true;
}

bool SecurityManager::ShouldBlockMixedContent(const std::string& url, const std::string& source_url) {
    if (!https_only_mode_ && security_level_ == SecurityLevel::LOW) {
        return false;  // Don't block mixed content in low security mode
    }
    
    // Check if source URL is HTTPS and target URL is HTTP
    bool source_is_https = StartsWith(ToLower(source_url), "https://");
    bool target_is_http = StartsWith(ToLower(url), "http://");
    
    if (source_is_https && target_is_http) {
        LOG_DEBUG("Blocking mixed content: HTTPS page loading HTTP resource");
        return true;
    }
    
    return false;
}

bool SecurityManager::ShouldAllowScriptExecution(const std::string& url, const std::string& source_url) {
    // Check URL safety
    SecurityCheckResult safety_result = CheckUrlSafety(url, source_url);
    if (!safety_result.is_safe) {
        LOG_DEBUG("Script execution blocked due to safety check: " + url);
        statistics_.scripts_blocked++;
        return false;
    }
    
    // Check for inline script restrictions based on CSP
    if (csp_enforcement_ && csp_manager_) {
        // This would check against the CSP of the source page
        // For now, we'll just log it
        LOG_DEBUG("Script execution allowed (CSP check would be here): " + url);
    }
    
    return true;
}

SecurityLevel SecurityManager::GetSecurityLevel() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return security_level_;
}

void SecurityManager::SetSecurityLevel(SecurityLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    security_level_ = level;
    LOG_INFO("Security level set to: " + std::to_string(static_cast<int>(level)));
}

void SecurityManager::SetHttpsOnlyMode(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    https_only_mode_ = enabled;
    LOG_INFO("HTTPS only mode " + std::string(enabled ? "enabled" : "disabled"));
}

void SecurityManager::SetCertificatePinning(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    certificate_pinning_ = enabled;
    LOG_INFO("Certificate pinning " + std::string(enabled ? "enabled" : "disabled"));
}

void SecurityManager::SetCspEnforcement(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    csp_enforcement_ = enabled;
    LOG_INFO("CSP enforcement " + std::string(enabled ? "enabled" : "disabled"));
}

void SecurityManager::SetSafeBrowsing(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    safe_browsing_ = enabled;
    LOG_INFO("Safe browsing " + std::string(enabled ? "enabled" : "disabled"));
}

void SecurityManager::AddPinnedCertificate(const std::string& hostname, const std::vector<uint8_t>& cert_data) {
    std::lock_guard<std::mutex> lock(mutex_);
    pinned_certificates_[hostname] = cert_data;
    LOG_INFO("Added pinned certificate for: " + hostname);
}

void SecurityManager::RemovePinnedCertificate(const std::string& hostname) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = pinned_certificates_.find(hostname);
    if (it != pinned_certificates_.end()) {
        pinned_certificates_.erase(it);
        LOG_INFO("Removed pinned certificate for: " + hostname);
    }
}

bool SecurityManager::HasPinnedCertificate(const std::string& hostname) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pinned_certificates_.find(hostname) != pinned_certificates_.end();
}

CertificateVerifier* SecurityManager::GetCertificateVerifier() {
    return cert_verifier_.get();
}

SandboxPolicy* SecurityManager::GetSandboxPolicy() {
    return sandbox_policy_.get();
}

ContentSecurityPolicy* SecurityManager::GetCspManager() {
    return csp_manager_.get();
}

void SecurityManager::ReportSecurityIssue(const std::string& url, ThreatType threat_type, 
                                          const std::string& details) {
    LOG_WARNING("Security issue reported - URL: " + url + 
                ", Type: " + std::to_string(static_cast<int>(threat_type)) + 
                ", Details: " + details);
    
    // In a real implementation, this would report to a security service
    // or add to a local database of known threats
}

SecurityManager::SecurityStatistics SecurityManager::GetStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return statistics_;
}

void SecurityManager::ResetStatistics() {
    std::lock_guard<std::mutex> lock(mutex_);
    statistics_ = {0, 0, 0, 0, 0};
}

void SecurityManager::InitializeDefaultSettings() {
    // Initialize default pinned certificates for important sites
    // In a real implementation, these would be the actual certificate hashes
    // AddPinnedCertificate("example.com", example_cert_data);
    
    LOG_INFO("Default security settings initialized");
}

void SecurityManager::LoadMaliciousUrlDatabase() {
    LOG_INFO("Loading malicious URL database");
    
    // In a real implementation, this would load from a file or database
    // For now, we'll add some example malicious URLs
    
    // Example: known malicious domains
    malicious_urls_.insert("malicious-site.com");
    malicious_urls_.insert("phishing-example.net");
    malicious_urls_.insert("fake-bank-site.org");
    
    LOG_INFO("Loaded " + std::to_string(malicious_urls_.size()) + " malicious URLs");
}

bool SecurityManager::IsKnownMaliciousUrl(const std::string& url) const {
    std::string domain = ExtractDomain(url);
    if (domain.empty()) {
        return false;
    }
    
    // Check if domain or any subdomain is in the malicious list
    for (const auto& malicious_domain : malicious_urls_) {
        if (EqualsIgnoreCase(domain, malicious_domain) ||
            EndsWith(domain, "." + malicious_domain)) {
            return true;
        }
    }
    
    return false;
}

bool SecurityManager::IsHttpsOnlyViolation(const std::string& url) const {
    if (!https_only_mode_) {
        return false;
    }
    
    std::string lower_url = ToLower(url);
    return StartsWith(lower_url, "http://") && !StartsWith(lower_url, "https://");
}

}  // namespace nullware
