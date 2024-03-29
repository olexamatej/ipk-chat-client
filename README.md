#TODO

fix ctrl+c to send bye packet


Proper received message writeup
Check if multiple clients on server work - Fix message receiving 
create another constructor for MSG, or find other way to check if received messages are correct - you made legalCheck, just dont exit on invalid

read until \r\n, split the messages //mby skip
add memory freeing on exit (mby its needed only in UDP)
change error to be printed to std::cerr
[A-Z]|[a-z]|[0-9]| - username, channelid and secret must follow this grammar

test TCP
TCP Packet receiver, add exit on error/bye
Check if multiple clients on server work - Fix message receiving 
program exits on using invalid chars but probs shouldnt, check it



MEELEY CHECKER FOR TCP

Add UDP message ID checker std::map ? works probs - probs doesnt work for auth and join packets
check if confirm messages are correct ID
UDP Errorpacket send
add sending BYE packet in udp
error when getting reply without sending join:3
fix rename
check if received messages are correct


start->auth works
auth->auth idk, test it
auth->open works
open->open works
open->error works for invalid message
error->end works
open->end works
open->end via bye probably doesnt work  