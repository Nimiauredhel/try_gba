
mod_time=$(stat -c "%Y" source/main.c)
echo "Mod time is $mod_time"
make
mgba-qt build/try.gba &
gba_pid=$!
echo "PID is $gba_pid"

while :
do

	temp_mod_time=$(stat -c "%Y" source/main.c)

	if [ $temp_mod_time -gt $mod_time ]; then
		mod_time=$temp_mod_time
		echo "Mod time is $mod_time"
		kill -9 $gba_pid
		make
		mgba-qt build/try.gba &
		gba_pid=$!
		echo "PID is $gba_pid"
	fi

	sleep 3

done
