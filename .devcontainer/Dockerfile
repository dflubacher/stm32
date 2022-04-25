# [Choice] Debian / Ubuntu version (use Debian 11, Ubuntu 18.04/21.04 on local arm64/Apple Silicon): debian-11, debian-10, ubuntu-21.04, ubuntu-20.04, ubuntu-18.04
ARG VARIANT=debian-11
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

ARG USERNAME=vscode
ARG BUILDDIR="/tmp/build"
ARG PYTHON_VER="3.6.15"
ARG OPENOCD_SCRIPTS="/usr/share/openocd/scripts"

# [Optional] Install CMake version different from what base image has already installed. 
# CMake reinstall choices: none, 3.21.5, 3.22.2, or versions from https://cmake.org/download/
ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.23.1"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/
RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

COPY ./custom-scripts/* /tmp/custom-scripts/

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get remove binutils-arm-none-eabi gcc-arm-none-eabi \
    && apt-get update \
    && apt-get install -y unzip curl

# https://steelph0enix.github.io/posts/vscode-cubemx-setup/
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get remove binutils-arm-none-eabi gcc-arm-none-eabi \
    && apt-get update && apt-get -y upgrade \
    && apt-get -y install \
        # https://forums.docker.com/t/how-can-i-install-python-3-6-version-on-top-of-r/68867/4
        wget gcc make zlib1g-dev \
        # For debugging use the GNU debugger:
        # gdb-multiarch \
        # Install ST Link utilities:
        # https://packages.debian.org/bullseye/stlink-tools \
        # https://github.com/stlink-org/stlink
        stlink-tools \
        # On Chip Debugger: OpenOCD:
        openocd \
        clangd \
        clang-format \
        # arm-none-eabi-gdb --version asked for this:
        libncursesw5 \
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

WORKDIR ${BUILDDIR}
# Would have liked to do this in a virtual environment, however gcc fails.
RUN wget --quiet https://www.python.org/ftp/python/${PYTHON_VER}/Python-${PYTHON_VER}.tgz > /dev/null 2>&1 \
    && tar zxf Python-${PYTHON_VER}.tgz \
    && cd Python-${PYTHON_VER} \
    && ./configure --enable-shared > /dev/null 2>&1 \
    && make > /dev/null 2>&1 \
    && make install > /dev/null 2>&1 \
    && rm -rf ${BUILDDIR} \
    # This is a hack, but it seems the gdb executable requires libpython3.6.
    && cp /usr/local/lib/libpython3.6m.so.1.0 /usr/lib/x86_64-linux-gnu/ \
    # Stop Python3.6 from polluting the PATH.
    && rm -rf /usr/local/bin/py*

# For some reason, Python3.6 is chosen with priority 1.
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.9 1

WORKDIR /

RUN chown ${USERNAME}:root /tmp/custom-scripts/*.sh \
    && chmod a+x /tmp/custom-scripts/*.sh \
    && chsh -s /usr/bin/zsh ${USERNAME} \
    # https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html#permission-issues-dev-ttyusb0
    && usermod -a -G dialout ${USERNAME} \
    && usermod -a -G plugdev ${USERNAME}


RUN curl -Lo /tmp/gcc-arm-86_x64-arm-none-eabi.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi.tar.xz" \
    && tar -xf /tmp/gcc-arm-86_x64-arm-none-eabi.tar.xz --directory /opt/ \
    && chown ${USERNAME}:root /opt/gcc-arm-*

USER ${USERNAME}

ENV PATH=/opt/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi/bin:\
/opt/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi/arm-none-eabi/bin:${PATH}

ENV OPENOCD_SCRIPTS=${OPENOCD_SCRIPTS}

RUN zsh -c "sed -i 's/ZSH_THEME.*/ZSH_THEME="eastwood"/1' ${HOME}/.zshrc"

RUN /tmp/custom-scripts/install_pip.sh && python3 -m pip install --user -U compiledb
