# Script to build all of the dependencies from scratch in the event that
# nothing has been previously established.

me=$(whoami)

echo "[INFO] Running script as ${me}"

original_dir=$(pwd)

DEP_DIR="../MIToolbox"

if [ -d "$DEP_DIR" ]; then
    if [ "$(ls -A $DEP_DIR)" ]; then
	echo "[DEBUG] Submodule already initialized";
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
    echo "[INFO] Running make install as root";
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
    echo "[INFO] Running make install as root";
    sudo make install;
fi

if [[ -z "${LD_LIBRARY_PATH}" ]] ; then
    # These were more than likely added to /usr/local/lib
    export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib;    
else
    if [[ ":$LD_LIBRARY_PATH:" == *":/lib:"* ]] ; then
	echo "[DEBUG] /lib is already part of LD_LIBRARY_PATH"
    else
	export LD_LIBRARY_PATH=/lib:$LD_LIBRARY_PATH
    fi

    if [[ ":$LD_LIBRARY_PATH:" == *":/usr/lib:"* ]] ; then
	echo "[DEBUG] /usr/lib is already part of LD_LIBRARY_PATH"
    else
	export LD_LIBRARY_PATH=/usr/lib:$LD_LIBRARY_PATH
    fi

    if [[ ":$LD_LIBRARY_PATH:" == *":/usr/local/lib:"* ]] ; then
	echo "[DEBUG] /usr/local/lib is already part of LD_LIBRARY_PATH"
    else
	export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
    fi
fi


# build the python data !
cd $original_dir;

if [ "$me" = "root" ] ; then
    python3 -m pip install --upgrade;
else
    echo "[INFO] Running python install as root";
    sudo python3 -m pip install . --upgrade;
fi
