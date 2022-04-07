# Script to build all of the dependencies from scratch in the event that
# nothing has been previously established.

original_dir=$(pwd)

DEP_DIR="../MIToolbox"

if [ -d "$DEP_DIR" ]; then
    if [ "$(ls -A $DEP_DIR)" ]; then
	echo "Submodule already initialized";
    else
	cd ..;
	git submodule init;
	git submodule update;
	cd -;
    fi
fi

# build the dependencies first
cd $DEP_DIR;
make clean && make && sudo make install;

# Move to the home directory
cd ..;
make clean && make && sudo make install;

# These were more than likely added to /usr/local/lib
export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib;

# build the python data !
cd $original_dir;
python3 -m pip install . --upgrade;
