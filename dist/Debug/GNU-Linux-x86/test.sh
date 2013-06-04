#!/bin/bash

pid=$(ps ux | awk '/port/ && !/awk/ {print $2}');

echo $pid;
