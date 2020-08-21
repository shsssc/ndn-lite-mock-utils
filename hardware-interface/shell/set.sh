#!/bin/bash
echo name: $1
echo key: $2
echo value $3
mkdir -p /tmp/ndnlite-mock/$1
printf $3 > /tmp/ndnlite-mock/$1/$2

