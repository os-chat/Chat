#!/bin/bash

var="http://whatever/score/"
temp_var="${var#http://}"
echo "$temp_var"

current_git=($(git config --get remote.origin.url)".git")
current_git="${current_git#https://}"
echo "$current_git"
