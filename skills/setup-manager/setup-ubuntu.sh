#!/usr/bin/env bash
set -euo pipefail

echo "[1/7] Updating package list"
sudo apt-get update

echo "[2/7] Installing build tools"
sudo apt-get install curl zip unzip tar -y
sudo apt-get install -y build-essential cmake ninja-build pkg-config git curl wget unzip tar gdb

echo "[3/7] Installing Qt"
sudo apt-get install -y qt6-base-dev qt6-tools-dev || sudo apt-get install -y qtbase5-dev qttools5-dev

echo "[4/7] Installing OpenCV"
sudo apt-get install -y libopencv-dev python3-opencv

echo "[5/7] Installing vcpkg"
if [ ! -d "$HOME/vcpkg" ]; then
  git clone https://github.com/microsoft/vcpkg.git "$HOME/vcpkg"
fi
"$HOME/vcpkg/bootstrap-vcpkg.sh"

echo "[6/7] Installing Docker"
sudo apt-get install -y ca-certificates curl gnupg lsb-release
sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
printf 'deb [arch=%s signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu %s stable\n' "$(dpkg --print-architecture)" "$(lsb_release -cs)" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
sudo usermod -aG docker "$USER"

echo "[7/7] Installing VS Code extensions"
if command -v code >/dev/null 2>&1; then
  code --install-extension ms-vscode.cpptools >/dev/null 2>&1 || true
  code --install-extension ms-vscode.cmake-tools >/dev/null 2>&1 || true
  code --install-extension twxs.cmake >/dev/null 2>&1 || true
  code --install-extension ms-vscode.cpptools-extension-pack >/dev/null 2>&1 || true
  code --install-extension ms-azuretools.vscode-docker >/dev/null 2>&1 || true
  code --install-extension eamodio.gitlens >/dev/null 2>&1 || true
  code --install-extension ms-vscode-remote.remote-wsl >/dev/null 2>&1 || true
fi

echo "Setup script completed. Please restart your shell or log out/in to refresh Docker permissions."
