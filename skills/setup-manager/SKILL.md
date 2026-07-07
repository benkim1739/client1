---
name: setup-manager
description: Use this skill to install and configure the development environment for the client project on Ubuntu or WSL2, including compiler tools, CMake, Qt, OpenCV, vcpkg, Docker, and VS Code.
---

# Setup Manager Skill

## Purpose
This skill prepares the local environment for the Client project so it can be built, debugged, and tested according to REQUIREMENTS.md.

## 1. Required Software
The target environment should include:
- Ubuntu or WSL2
- g++ and gdb
- CMake and Ninja
- Git, curl, wget, unzip, tar, pkg-config
- Qt
- OpenCV
- vcpkg
- Docker
- VS Code with relevant extensions

## 2. Installation Steps

### 2.1 Update system packages
Run the following commands:

```bash
sudo apt-get update
sudo apt-get upgrade -y
```

### 2.2 Install core build tools
```bash
sudo apt-get install -y build-essential cmake ninja-build pkg-config git curl wget unzip tar
```

### 2.3 Install debugging tools
```bash
sudo apt-get install -y gdb
```

### 2.4 Install Qt
```bash
sudo apt-get install -y qt6-base-dev qt6-tools-dev
```

If the environment does not provide Qt 6 packages, use Qt 5 as a fallback:

```bash
sudo apt-get install -y qtbase5-dev qttools5-dev
```

### 2.5 Install OpenCV
```bash
sudo apt-get install -y libopencv-dev python3-opencv
```

### 2.6 Install vcpkg
```bash
cd ~
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
```

Add the binary path to the shell profile if needed:

```bash
echo 'export PATH="$HOME/vcpkg:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

### 2.7 Install Docker
```bash
sudo apt-get install -y ca-certificates curl gnupg lsb-release
sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
sudo usermod -aG docker $USER
```

### 2.8 Configure VS Code
Install the following extensions:

```bash
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cmake-tools
code --install-extension twxs.cmake
code --install-extension ms-vscode.cpptools-extension-pack
code --install-extension ms-azuretools.vscode-docker
code --install-extension eamodio.gitlens
code --install-extension ms-vscode-remote.remote-wsl
```

The workspace already includes recommended VS Code settings in .vscode/settings.json and .vscode/extensions.json.

## 3. Verification
After installation, verify the environment with:

```bash
g++ --version
cmake --version
ninja --version
gdb --version
pkg-config --version
opencv_version
```

For Docker, verify with:

```bash
docker --version
```

## 4. Notes
- If the machine is running inside WSL2, install the tools inside the Linux distro, not the Windows host.
- Reboot or restart the shell after changing group memberships such as Docker access.
- Keep package installation commands reproducible by recording changes in the project documentation.
