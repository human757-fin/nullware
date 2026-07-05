#ifndef NULLWARE_SECURITY_SECURITY_MANAGER_H_
#define NULLWARE_SECURITY_SECURITY_MANAGER_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <mutex>

#include "src/common/config.h"

namespace nullware {

class CertificateVerifier;
class SandboxPolicy;
class ContentSecurityPolicy;

// Security levels
enum class SecurityLevel {
    LOW,
    MEDIUM,
    HIGH,
    CUSTOM
};

// Security threat types
enum class ThreatType {
    NONE,
    MALWARE,
    PHISHING,
    SOCIAL_ENGINEERING,
    UNWANTED_SOFTWARE,
    POTENTIALLY_HARMFUL_APPLICATION,
    SAFE_BROWSING_MALWARE,
    SAFE_BROWSING_SOCIAL_ENGINEERING,
    SAFE_BROWSING_UNWANTED,
    SAFE_BROWSING_HARMFUL
};

// Security check result
struct SecurityCheckResult {
    bool is_safe;
    ThreatType threat_type;
    std::string threat_url;
    std::string details;
    SecurityLevel required_level;
};

// Security manager for handling all security-related functionality
class SecurityManager {
public:
    SecurityManager();
    ~SecurityManager();
    
    // Initialize the security manager
    bool Initialize();
    
    // Shutdown the security manager
    void Shutdown();
    
    // Check if a URL is safe to visit
    SecurityCheckResult CheckUrlSafety(const std::string& url, const std::string& source_url = "");
    
    // Check if a certificate is valid
    bool VerifyCertificate(const std::string& hostname, const std::vector<uint8_t>& cert_data);
    
    // Check Content Security Policy
    bool CheckContentSecurityPolicy(const std::string& url, const std::string& csp_header);
    
    // Check if a request should be allowed
    bool ShouldAllowRequest(const std::string& url, const std::string& method,
                           const std::vector<std::pair<std::string, std::string>>& headers,
                           const std::string& source_url = "");
    
    // Check if mixed content should be blocked
    bool ShouldBlockMixedContent(const std::string& url, const std::string& source_url);
    
    // Check if a script should be executed
    bool ShouldAllowScriptExecution(const std::string& url, const std::string& source_url);
    
    // Get the current security level
    SecurityLevel GetSecurityLevel() const;
    
    // Set the security level
    void SetSecurityLevel(SecurityLevel level);
    
    // Enable or disable specific security features
    void SetHttpsOnlyMode(bool enabled);
    void SetCertificatePinning(bool enabled);
    void SetCspEnforcement(bool enabled);
    void SetSafeBrowsing(bool enabled);
    
    // Add a pinned certificate
    void AddPinnedCertificate(const std::string& hostname, const std::vector<uint8_t>& cert_data);
    
    // Remove a pinned certificate
    void RemovePinnedCertificate(const std::string& hostname);
    
    // Check if a hostname has a pinned certificate
    bool HasPinnedCertificate(const std::string& hostname) const;
    
    // Get certificate verifier
    CertificateVerifier* GetCertificateVerifier();
    
    // Get sandbox policy
    SandboxPolicy* GetSandboxPolicy();
    
    // Get CSP manager
    ContentSecurityPolicy* GetCspManager();
    
    // Report a security issue
    void ReportSecurityIssue(const std::string& url, ThreatType threat_type, 
                             const std::string& details);
    
    // Get security statistics
    struct SecurityStatistics {
        size_t urls_checked;
        size_t threats_detected;
        size_t certificates_verified;
        size_t mixed_content_blocked;
        size_t scripts_blocked;
    };
    
    SecurityStatistics GetStatistics() const;
    
    // Reset statistics
    void ResetStatistics();
    
private:
    std::unique_ptr<CertificateVerifier> cert_verifier_;
    std::unique_ptr<SandboxPolicy> sandbox_policy_;
    std::unique_ptr<ContentSecurityPolicy> csp_manager_;
    
    SecurityLevel security_level_;
    bool https_only_mode_;
    bool certificate_pinning_;
    bool csp_enforcement_;
    bool safe_browsing_;
    
    SecurityStatistics statistics_;
    mutable std::mutex mutex_;
    
    // Pinned certificates (hostname -> certificate data)
    std::unordered_map<std::string, std::vector<uint8_t>> pinned_certificates_;
    
    // Known malicious URLs
    std::unordered_set<std::string> malicious_urls_;
    
    // Initialize default security settings
    void InitializeDefaultSettings();
    
    // Load known malicious URLs
    void LoadMaliciousUrlDatabase();
    
    // Check URL against known malicious URLs
    bool IsKnownMaliciousUrl(const std::string& url) const;
    
    // Check for HTTPS only mode violations
    bool IsHttpsOnlyViolation(const std::string& url) const;
};

// Global security manager instance
class SecurityManagerSingleton {
public:
    static SecurityManager& GetInstance() {
        static SecurityManager instance;
        return instance;
    }
    
private:
    SecurityManagerSingleton() = default;
    ~SecurityManagerSingleton() = default;
};

}  // namespace nullware

#endif  // NULLWARE_SECURITY_SECURITY_MANAGER_H_
