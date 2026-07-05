# CEF Download Instructions for NullWare Browser

Since automatic downloading is restricted in this environment, please follow these manual steps to download CEF (Chromium Embedded Framework) for building NullWare Browser.

## 📥 **Manual CEF Download**

### **Step 1: Visit CEF Download Page**
Open your web browser and go to:
👉 [https://cef-builds.spotifycdn.com/index/](https://cef-builds.spotifycdn.com/index/)

### **Step 2: Find Your Platform Version**

Look for files matching your platform:

| Platform | File Pattern | Example |
|----------|--------------|---------|
| **Linux 64-bit** | `cef_binary_*_linux64_minimal.tar.bz2` | `cef_binary_115.7.180+ge06b462+chromium-115.0.5790.170_linux64_minimal.tar.bz2` |
| **macOS 64-bit** | `cef_binary_*_macosx64_minimal.tar.bz2` | `cef_binary_115.7.180+ge06b462+chromium-115.0.5790.170_macosx64_minimal.tar.bz2` |
| **Windows 64-bit** | `cef_binary_*_windows64_minimal.tar.bz2` | `cef_binary_115.7.180+ge06b462+chromium-115.0.5790.170_windows64_minimal.tar.bz2` |

**Important:** Download the **minimal** version, not the full SDK.

### **Step 3: Download the File**

1. Click on the appropriate file for your platform
2. Save it to your computer
3. Remember where you saved it

### **Step 4: Extract to Project**

#### **On Linux/macOS:**
```bash
# Navigate to your NullWare project
cd /path/to/nullware

# Create third_party directory if it doesn't exist
mkdir -p third_party

# Move the downloaded file to third_party
mv /path/to/downloaded/file.tar.bz2 third_party/

# Extract the archive
cd third_party
tar -xf file.tar.bz2

# Remove the archive (optional)
rm file.tar.bz2

# Verify extraction
ls -la cef/
```

#### **On Windows:**
1. Navigate to your NullWare project folder
2. Create a `third_party` folder if it doesn't exist
3. Move the downloaded `.tar.bz2` file into the `third_party` folder
4. Extract the archive using 7-Zip, WinRAR, or Windows built-in extractor
5. You should see a `cef` folder inside `third_party`

### **Step 5: Verify CEF Structure**

After extraction, your `third_party/cef/` directory should contain:
```
cef/
├── include/          # CEF header files
├── lib/             # CEF libraries
│   └── Release/     # Release build libraries
├── Resources/       # Resource files
├── tests/           # Test files
└── README.md        # CEF readme
```

## 🏗️ **Build NullWare Browser**

Once CEF is downloaded and extracted:

```bash
# Navigate to project root
cd /path/to/nullware

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the browser
cmake --build . -j$(nproc)

# Run the browser
./bin/nullware
```

## 🔍 **Troubleshooting**

### **CEF Version Mismatch**
If you get errors about CEF version, make sure:
1. You downloaded the **minimal** version
2. The architecture matches your system (64-bit)
3. The CEF version is compatible with your compiler

### **Missing Dependencies**
On Linux, you may need additional dependencies:
```bash
# Ubuntu/Debian
sudo apt-get install -y cmake build-essential libgtk-3-dev libnss3-dev libx11-dev

# Fedora
sudo dnf install -y cmake gcc-c++ gtk3-devel nss-devel libX11-devel

# Arch Linux
sudo pacman -S --needed cmake gcc gtk3 nss libx11
```

### **Build Errors**
If you encounter build errors:
1. Make sure CEF is in `third_party/cef/`
2. Check that the `include/` directory exists inside `cef/`
3. Verify that `cef/include/cef_base.h` exists
4. Try cleaning the build directory and rebuilding

## 📋 **Alternative: Use System-Wide CEF**

If you have CEF installed system-wide, you can skip the download:

```bash
# Configure to use system CEF
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_SYSTEM_CEF=ON

# Build
cmake --build . -j$(nproc)
```

## 🌐 **CEF Version Information**

| Version | Chromium Version | Release Date | Notes |
|---------|------------------|--------------|-------|
| 125.7.10 | 125.0.6422.60 | 2024-05-14 | Latest stable |
| 123.7.1 | 123.0.6312.105 | 2024-04-09 | Recommended |
| 121.7.1 | 121.0.6167.139 | 2024-03-12 | Stable |
| 115.7.180 | 115.0.5790.170 | 2023-10-10 | Well-tested |

**Recommendation:** Use version **115.7.180** as it's well-tested and widely compatible.

## 📞 **Support**

If you're having trouble downloading or building CEF:
1. Check the [CEF GitHub repository](https://github.com/chromiumembedded/cef)
2. Visit the [CEF Forum](https://magpcss.org/ceforum/)
3. Join the [CEF Discord](https://discord.gg/cef)

## ✅ **Checklist**

- [ ] Downloaded CEF minimal version for my platform
- [ ] Extracted to `third_party/cef/`
- [ ] Verified `cef/include/cef_base.h` exists
- [ ] Installed required system dependencies
- [ ] Built NullWare Browser successfully
- [ ] Ran the browser

---

**Need help?** The CEF community is very active and helpful. Don't hesitate to ask for assistance on their forums or Discord server!
