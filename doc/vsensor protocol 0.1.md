vsensor protocol
================

##General
The vsensor implements a UDP server that accepts interaction from any UDP client. The request/response
pairs are in clear text. The server verifies the request
and if valid returns a response to the client.

###Fields

- **Passphase**: an ASCII word not exceeding 16 characters
- **Command**: Q - query current status
- **SiteID**: the unique site identifier, an integer less than 1000
- **SiteType**: A = AC site, S = Solar site
- **RelayState**: N = Not implemented, C = closed, O = open
- **SensorReading**: an integer between 0 and 1023 with the sensor reading
- **SensorVoltage**: the voltage measured by the sensor, one decimal precision
- **ErrorCode**: request error response code. E00 = undefined error, E10 = invalid password

##Message detail

###1. Get Status

####Request:

	<Passphrase>,<Command>

For example:

	MyPassword,Q

####Response:

	<SiteID>,<SiteType>,<RelayStatus>,<SensorValue>,<SensorVoltage>
	
or 
	
	<SiteID>,<ErrorCode>

For example:

	045,A,C,800,26.5
	
or

	045,E10
	

