#!/bin/bash

num1=3
num2=4

# use let cmd
let result=num1+num2
echo $result

# self incr
let num1+=3

# self decr
let num2--

let new_ret=num1+num2
echo $new_ret
