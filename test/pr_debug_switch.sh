#!/bin/bash

case "$1" in
	"on" ) 
	echo pr_debug on
	echo 'module ztxmap +p' > /sys/kernel/debug/dynamic_debug/control 
	;;
	"off" )
	echo pr_debug off
	echo 'module ztxmap -p' > /sys/kernel/debug/dynamic_debug/control 
	;;
esac
