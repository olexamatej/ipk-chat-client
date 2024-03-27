#TODO

Proper received message writeup
test TCP
TCP Packet receiver, add exit on error/bye
Check if multiple clients on server work - Fix message receiving 
Add UDP message ID checker std::map ? works probs - probs doesnt work for auth and join packets
create another constructor for MSG, or find other way to check if received messages are correct
check if confirm messages are correct ID
read until \r\n, split the messages //this might not be a problem
fix rename
UDP Errorpacket send
program exits on using invalid chars but probs shouldnt, check it



MEELEY CHECKER FOR TCP

start->auth works
auth->auth idk, test it
auth->open works
open->open works
open->error works for invalid message
error->end works
open->end works
open->end via bye probably doesnt work  