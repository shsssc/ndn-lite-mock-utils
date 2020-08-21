#!/bin/bash
echo name: $1
echo key: $2
printf `cat /tmp/ndnlite-mock/$1/$2`

