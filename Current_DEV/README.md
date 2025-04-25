Current Dev plan
<p>
The tech is mostly platform agnostic, as long as whatever mcu youre using can communicate with the QM331x0w (either module or ic on its own) over SPI (6.81mbps) and with an imu in some way (doesnt matter interface) it can work.
As we're writing bits in the QM's memory buffer we can use any language on any node, even different languages on different nodes but probably not ideal lmao.
<p>
Original hardware was intended to be an stm32f411ce with a qm33120w and lsm6dsr imu but other mcus and imus should work fine (or even better), so try a few, see how they work.
<p>
We (I) want to make a relatively low cost tracking system to compete with the likes of slime, haitorax etc, unfortunately its just not gonna be as cheap as their mcu + imu because we have an additional relatively expensive component,
particularly with off the shelf hardware with the uwb ic onboard being expensive (the dwm3001cdk is probably cheapest and doesnt have an imu onboard) so if we can get a design working custom for this 
then we can get more people in the system and working/testing/using it.
<p>
<p>
Priority of dev:
<p>
Hardware design <br />
	Antares_Raw - this is working with the raw qm33120w and will yield the cheapest end hardware but because its raw rf would need to be certified to be sold to consumers
<p>
<p>
Ranging code running on any hardware <br>
	Code - has the python location engine, <br>
	datasheets - has the datasheets for the qm331x0w and dwm3001c so will be of use for figuring out where to write/read from on the ic's memory itself <br>
<p>
<p>
when we get here, we can worry about what's next