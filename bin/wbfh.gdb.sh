cd /home/tabajara/server
export LD_LIBRARY_PATH="./libs/"
screen -A -m -d -S wbserver gdb -d ./source -x gdb.commands wbserver
