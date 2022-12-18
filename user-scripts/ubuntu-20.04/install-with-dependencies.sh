# Dependencies
sudo apt install libwebkit2gtk-4.0-dev
sudo apt install clang
# sudo apt instal sudo apt install gstreamer1.0-plugins-good gstreamer1.0-libav

# Adblock
git clone https://github.com/jun7/wyebadblock
cd wyebadblock
make
sudo make install 
cd ..

# Rose config
user=$(whoami)
mkdir -p /home/$user/.cache/rose
cp ../../config.def.h ../../config.h # you should also probably customize this yourself.
sed "s/fenze/$user/g" ../../config.h

sudo bash ../../install.sh

# Ubuntu desktop icon
chmod +x rose.desktop
sudo cp rose.desktop /usr/share/applications
