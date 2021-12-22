# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.205.2/containers/cpp/.devcontainer/base.Dockerfile

# [Choice] Debian / Ubuntu version (use Debian 11/9, Ubuntu 18.04/21.04 on local arm64/Apple Silicon): debian-11, debian-10, debian-9, ubuntu-21.04, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="bullseye"
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>

RUN LC_ALL=en_US.UTF-8 apt-get update -qq \
    && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install -qq --no-install-recommends software-properties-common python3-software-properties \
    && apt-add-repository ppa:remmina-ppa-team/remmina-next-daily -y \
    && apt-get update -qq \
    && apt-get -y install -qq --no-install-recommends build-essential git-core \
        cmake curl freerdp2-dev intltool libappindicator3-dev libasound2-dev \
        libavahi-ui-gtk3-dev libavcodec-dev libavresample-dev libavutil-dev \
        libcups2-dev libgcrypt20-dev libgnutls28-dev \
        libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev libgtk-3-dev \
        libjpeg-dev libjson-glib-dev libpcre2-8-0 libpcre2-dev libpulse-dev \
        libsecret-1-dev libsodium-dev libsoup2.4-dev libspice-client-gtk-3.0-dev \
        libspice-protocol-dev libssh-dev libssl-dev libsystemd-dev \
        libtelepathy-glib-dev libvncserver-dev libvte-2.91-dev libwebkit2gtk-4.0-dev \
        libx11-dev libxcursor-dev libxdamage-dev libxext-dev libxi-dev \
        libxinerama-dev libxkbfile-dev libxkbfile-dev libxml2 libxml2-dev \
        libxrandr-dev libxtst-dev libxv-dev python3 python3-dev wget \
    && apt-get autoremove -y \
    && apt-get clean -y


# RUN apt-get install libayatana-indicator3-dev
# RUN apt-get install libayatana-appindicator3-dev
# RUN apt-get install clang-format
# RUN apt-get install xdg-utils
# RUN apt-get install libgtkmm-3.0-dev
