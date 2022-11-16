#!/bin/bash -eux

# shellcheck disable=SC2140
printf "%b" "AJA_DIRECTORY=/var/tmp/ntv2\n"\
"CPATH=/usr/local/qt/include\n"\
"LIBRARY_PATH=/usr/local/qt/lib\n"\
"PKG_CONFIG_PATH=/usr/local/qt/lib/pkgconfig\n" >> "$GITHUB_ENV"
printf "/usr/local/qt/bin\n" >> "$GITHUB_PATH"

sed -n '/^deb /s/^deb /deb-src /p' /etc/apt/sources.list | sudo tee /etc/apt/sources.list.d/sources.list # for build-dep ffmpeg
sudo apt update
sudo apt -y upgrade
sudo apt install appstream # appstreamcli for mkappimage AppStream validation
sudo apt install aptitude
sudo apt install fonts-dejavu-core
sudo apt install libcppunit-dev
sudo apt --no-install-recommends install nvidia-cuda-toolkit
sudo apt install libglew-dev libglfw3-dev
sudo apt install libglm-dev
sudo apt install libx11-dev
sudo apt install libsoxr-dev libspeexdsp-dev
sudo apt install libssl-dev
sudo apt install libasound-dev libjack-jackd2-dev libnatpmp-dev libv4l-dev portaudio19-dev
sudo apt install libopencv-core-dev libopencv-imgproc-dev
sudo apt install libcurl4-nss-dev
sudo apt install i965-va-driver-shaders # instead of i965-va-driver
sudo apt install uuid-dev # Cineform

sudo aptitude -y build-dep libsdl2 libsdl2-mixer libsdl2-ttf libsdl2-dev:

# FFmpeg deps
sudo add-apt-repository ppa:savoury1/vlc3 # new x265
# updates nasm 2.13->2.14 in U18.04 (needed for rav1e)
update_nasm() {
        if [ -z "$(apt-cache search --names-only '^nasm-mozilla$')" ]; then
                return
        fi
        sudo apt install nasm- nasm-mozilla
        sudo ln -s /usr/lib/nasm-mozilla/bin/nasm /usr/bin/nasm
}
# for FFmpeg - libzmq3-dev needs to be ignored (cannot be installed, see run #380)
sudo aptitude -y build-dep ffmpeg libsdl2-dev: libzmq3-dev:
sudo apt install libdav1d-dev
sudo apt-get -y remove 'libavcodec*' 'libavutil*' 'libswscale*' libvpx-dev 'libx264*' nginx
update_nasm
# own x264 build
sudo apt --no-install-recommends install asciidoc xmlto

sudo apt install qtbase5-dev

# Install cross-platform deps
"$GITHUB_WORKSPACE/.github/scripts/install-common-deps.sh"

"$GITHUB_WORKSPACE/.github/scripts/Linux/install_others.sh"

