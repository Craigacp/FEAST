# Script to build all of the dependencies from scratch in the event that
# nothing has been previously established.
set -eux

function LOG() {
    echo -e "\033[0;34m${1}\033[0m"
}

me=$(whoami)

LOG "[INFO] Running script as ${me}"

original_dir=$(pwd)

DEP_DIR="../MIToolbox"

if [ -d "$DEP_DIR" ]; then
    if [ "$(ls -A $DEP_DIR)" ]; then
	LOG "Submodule already initialized";
    else
	cd ..;
	git submodule init;
	git submodule update;
	cd -;
    fi
fi

# build the dependencies first
cd $DEP_DIR;
make clean && make;

if [ "$me" = "root" ] ; then
    LOG "Running make install as root";
    make install;
else
    sudo make install;
fi

# Move to the home directory
cd ..;
make clean && make;

if [ "$me" = "root" ] ; then
    make install;
else
    LOG "Running make install as root";
    sudo make install;
fi

if [[ -z "${LD_LIBRARY_PATH}" ]] ; then
    # These were more than likely added to /usr/local/lib
    LOG "export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib" >> $HOME/.bashrc
else
    if [[ ":$LD_LIBRARY_PATH:" == *":/lib:"* ]] ; then
	LOG "/lib is already part of LD_LIBRARY_PATH"
    else
	LOG "export LD_LIBRARY_PATH=/lib:$LD_LIBRARY_PATH" >> $HOME/.bashrc
    fi

    if [[ ":$LD_LIBRARY_PATH:" == *":/usr/lib:"* ]] ; then
	LOG "/usr/lib is already part of LD_LIBRARY_PATH"
    else
	LOG "export LD_LIBRARY_PATH=/usr/lib:$LD_LIBRARY_PATH" >> $HOME/.bashrc
    fi

    if [[ ":$LD_LIBRARY_PATH:" == *":/usr/local/lib:"* ]] ; then
	LOG "/usr/local/lib is already part of LD_LIBRARY_PATH"
    else
	LOG "export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH" >> $HOME/.bashrc
    fi
fi

# build the python data !
cd $original_dir;

if [ "$me" = "root" ] ; then
    python3 -m pip install . --upgrade;
else
    LOG "Running python install as root";
    sudo python3 -m pip install . --upgrade;
fi
