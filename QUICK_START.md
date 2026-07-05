# NullWare Browser - Quick Start Guide

## 🚀 **Fastest Way to Get Started**

### **Step 1: Download CEF Manually**

Since automatic downloading is restricted, here are the **direct download links** for CEF:

#### **Linux 64-bit**
```bash
# Run these commands:
cd nullware
mkdir -p third_party
cd third_party

# Download CEF (choose one version):
# Option A: Version 115.7.180 (Recommended - well-tested)
wget https://cef-builds.spotifycdn.com/cef_binary_115.7.180%2Bge06b462%2Bchromium-115.0.5790.170_linux64_minimal.tar.bz2

# Option B: Version 105.7.14 (Older but stable)
# wget https://cef-builds.spotifycdn.com/cef_binary_105.7.14%2Bg475f572%2Bchromium-105.0.5195.127_linux64_minimal.tar.bz2

# Extract CEF
tar -xf cef_binary_*_linux64_minimal.tar.bz2

# Clean up
rm cef_binary_*_linux64_minimal.tar.bz2

# Verify
ls -la cef/include/cef_base.h
```

#### **macOS 64-bit**
```bash
# Run these commands:
cd nullware
mkdir -p third_party
cd third_party

# Download CEF
wget https://cef-builds.spotifycdn.com/cef_binary_115.7.180%2Bge06b462%2Bchromium-115.0.5790.170_macosx64_minimal.tar.bz2

# Extract CEF
tar -xf cef_binary_*_macosx64_minimal.tar.bz2

# Clean up
rm cef_binary_*_macosx64_minimal.tar.bz2

# Verify
ls -la cef/include/cef_base.h
```

#### **Windows 64-bit**
1. Open your web browser
2. Visit: `https://cef-builds.spotifycdn.com/cef_binary_115.7.180%2Bge06b462%2Bchromium-115.0.5790.170_windows64_minimal.tar.bz2`
3. Download the file
4. Extract the `.tar.bz2` file to `nullware/third_party/`
5. You should see a `cef` folder inside `third_party`

---

## 🏗️ **Step 2: Install Dependencies**

### **Ubuntu/Debian**
```bash
sudo apt-get update
sudo apt-get install -y cmake build-essential libgtk-3-dev libnss3-dev libx11-dev libxcomposite-dev libxdamage-dev libxi-dev libxrandr-dev libxrender-dev libxtst-dev libasound2-dev libcups2-dev libfreetype6-dev libgbm-dev
```

### **Fedora**
```bash
sudo dnf install -y cmake gcc-c++ gtk3-devel nss-devel libX11-devel libXcomposite-devel libXdamage-devel libXi-devel libXrandr-devel libXrender-devel libXtst-devel alsa-lib-devel cups-devel freetype-devel libgbm-devel
```

### **Arch Linux**
```bash
sudo pacman -S --needed cmake gcc gtk3 nss libx11 libxcomposite libxdamage libxi libxrandr libxrender libxtst alsa-lib cups freetype2 libgbm
```

### **macOS**
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake gtk+3 nss
```

### **Windows**
1. Install **Visual Studio 2022** (Community Edition)
2. Install **CMake** from https://cmake.org/download/
3. Make sure both are in your PATH

---

## 🔨 **Step 3: Build the Browser**

```bash
# Navigate to project
cd nullware

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . -j$(nproc)

# On Windows:
cmake --build . --config Release
```

**Build Options:**
- `-DCMAKE_BUILD_TYPE=Release` - Optimized build (default)
- `-DCMAKE_BUILD_TYPE=Debug` - Debug build with symbols
- `-DENABLE_ADBLOCKER=OFF` - Disable ad-blocker
- `-DENABLE_DEVTOOLS=OFF` - Disable developer tools
- `-DENABLE_SANDBOX=OFF` - Disable sandboxing

---

## 🚀 **Step 4: Run the Browser**

```bash
# From build directory
./bin/nullware

# Or from project root
./build/bin/nullware
```

**Command Line Options:**
```bash
# Disable ad-blocker
./nullware --disable-adblocker

# Enable HTTPS-only mode
./nullware --https-only

# Use custom user data directory
./nullware --user-data-dir=/tmp/nullware-data

# Enable remote debugging
./nullware --remote-debugging

# All options:
./nullware --help
```

---

## 🎯 **Step 5: Test the Features**

### **UI Toggles**
- Click **🚫** button to toggle Ad-Blocker
- Click **👨‍💻** button to toggle DevTools
- Click **🔒** button to toggle HTTPS-Only mode
- Click **🛡️** button to toggle Sandbox

### **Tab Management**
- Click **+** button to create new tab
- Click **×** on a tab to close it
- Click on any tab to switch to it

### **Downloads**
- Click on download links (automatic)
- Click **⬇️** button to view downloads
- Manage downloads (pause, resume, cancel)

### **Settings**
- Click **⚙️** button to open settings
- Configure all browser options
- Apply changes immediately

---

## 🐛 **Troubleshooting**

### **CEF not found**
```
Error: CEF not found at third_party/cef
Solution: Make sure you downloaded and extracted CEF correctly
```

### **Missing cef_base.h**
```
Error: cef_base.h not found
Solution: Check that third_party/cef/include/cef_base.h exists
```

### **Build errors on Linux**
```
Error: Package not found
Solution: Install missing dependencies (see Step 2)
```

### **Build errors on Windows**
```
Error: Generator not found
Solution: Make sure Visual Studio 2022 is installed
```

### **CEF download fails**
```
Error: 404 Not Found
Solution: Try a different CEF version from the list above
```

---

## 📚 **Additional Resources**

- **Full Documentation**: [docs/DEVELOPMENT.md](docs/DEVELOPMENT.md)
- **UI Features**: [docs/UI_FEATURES.md](docs/UI_FEATURES.md)
- **CEF Download Instructions**: [CEF_DOWNLOAD_INSTRUCTIONS.md](CEF_DOWNLOAD_INSTRUCTIONS.md)
- **GitHub Repository**: [https://github.com/human757-fin/nullware](https://github.com/human757-fin/nullware)

---

## ✅ **Checklist**

- [ ] Downloaded CEF for my platform
- [ ] Extracted CEF to `third_party/cef/`
- [ ] Verified `third_party/cef/include/cef_base.h` exists
- [ ] Installed required dependencies
- [ ] Built NullWare Browser successfully
- [ ] Ran the browser
- [ ] Tested UI toggles
- [ ] Tested tab management
- [ ] Tested download functionality

---

## 🎉 **You're Ready!**

Once you've completed the steps above, you'll have a fully functional **NullWare Browser** with:

✅ Polished UI with all feature toggles  
✅ Tab support with create/close/switch  
✅ Download management  
✅ Ad-blocker with EasyList support  
✅ Security features (HTTPS-only, sandboxing, CSP)  
✅ Developer tools  
✅ Comprehensive settings  

**Enjoy your security and developer-friendly browser!** 🚀
