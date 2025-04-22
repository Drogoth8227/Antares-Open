# Antares-Open
Antares is a UWB based RTLS primarily focused on VR applications

(think lighthouse tracking but cheaper and using rf instead of light)


WARNING: very WIP, this is going open because i don't have the skill or time to build on my own,

I will help where I can with architecture and design but I cant do it all.


WARNING 2: currently no uploadable code, the location engine works in python, thats about as close as we have gotten so far


WARNING 3: UWB is still coming into the spotlight in mainstream devices (phones/headphones etc) so conflicts may occur and should be fixed ASAP


pros:

not line of sight based (blankets are back on the table)

no imu drift

solid state (no moving parts to wear out, looking at you lighthouses)

very low battery use, depending on hardware trackers could last weeks of usage per charge

open source- GNU public license so everyone benefits from updates


cons:

accuracy is untested (standard usage is within 10cm but can be improved with oversampling)

requires its own base stations (3 minimum, 4 suggested)

is still mostly theoretical at the moment (hopefully will change)

still costs money (estimations are around 60$ per tracker/base station)



progress so far:

system architecture drawn out

hardware schematic complete - layout TODO

location engine written in python - translate to other languages TODO

pc host client - not started

steamvr driver - not started


still want to get into it:

we currently have no custom hardware but you can order a dwm3001cdk from mouser/digikey and start with that

right now the priority is getting the location engine onto working hardware and demonstrate it works,

after that we can get into the features of UWB such as 1000hz polling, longer range, battery life, cost optimisation etc



hardware links

us digikey: https://www.digikey.com/en/products/detail/qorvo/DWM3001CDK/24367348?s=N4IgTCBcDaICYHcC2BmADGgjAYzgaxAF0BfIA

uk digikey: https://www.digikey.co.uk/en/products/detail/qorvo/DWM3001CDK/24367348?s=N4IgTCBcDaICYHcC2BmADGgjAYzgaxAF0BfIA



us mouser: https://www.mouser.com/ProductDetail/Qorvo/DWM3001CDK?qs=QNEnbhJQKvYHx2Hn8Cdgjg%3D%3D&_gl=1*7y67sj*_ga*dW5kZWZpbmVk*_ga_15W4STQT4T*dW5kZWZpbmVk*_ga_1KQLCYKRX3*dW5kZWZpbmVk

uk mouser: https://www.mouser.co.uk/ProductDetail/Qorvo/DWM3001CDK?qs=QNEnbhJQKvYHx2Hn8Cdgjg%3D%3D&_gl=1*7y67sj*_ga*dW5kZWZpbmVk*_ga_15W4STQT4T*dW5kZWZpbmVk*_ga_1KQLCYKRX3*dW5kZWZpbmVk

(mouser uk dont like selling rf to consumers for some reason, so be warned)
