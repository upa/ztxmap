#!/bin/bash

case "$1" in
	"on" ) 
	echo 'module ztxmap +p' > /sys/kernel/debug/dynamic_debug/control 
	;;
	"off" )
	echo 'module ztxmap -p' > /sys/kernel/debug/dynamic_debug/control 
	;;
esac
