# Simple script to automate compilation and execution
# echo "INFO: This script assumes that you have cloned the repository in your home directory."
echo "Compiling..."
src_dir=$(dirname "${BASH_SOURCE[${#BASH_SOURCE[@]} - 1]}")

# Check if a directory does not exist
if [ ! -d "${src_dir}/../build" ] 
then
    echo "Creating build folder..." 
    mkdir ${src_dir}/../build
fi

cc -o ${src_dir}/../build/app_tui ${src_dir}/../app/app_tui.c -lncurses -lm
echo "Compilation complete. Running executable... "
echo "======================================================="
${src_dir}/../build/app_tui