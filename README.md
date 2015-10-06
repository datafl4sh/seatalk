# seatalk
###Seatalk and Seatalk-NG tools.

To run the software you need a working Erlang environment and a
working Apache/PHP installation. In Debian is only a matter of
`apt-get install <something>`. You must also be able to build C code (`apt-get install build-essential`).

#### What you find in the folders

- `data`: some sample data collected during navigation. 'st' data is Seatalk, while 'stng' data is SeatalkNG aquired with actisense;
- `doc`: some documentation;
- `firmware`: PIC firmware used in the 9 to 8 bit data converter;
- `nav`: the web interface code. It connects to the translator (via TCP port 4545) and displays the received data;
- `serial`: a program that opens the serial port, configures it, reads data and forwards it to stdout;
- `translator`: all the Erlang stuff that translates from Seatalk(NG) to JSON format.

#### How to build the thing:
 
- `git clone https://github.com/datafl4sh/seatalk.git`
- `cd seatalk`
- `git clone https://github.com/davisp/jiffy.git`
- `sh ./build.sh`

#### How to run the thing with sample data:

- `cd translator`
- `./run-st.sh < ../data/testdata-st.txt` (For Seatalk)
- `./run-stng.sh < ../data/testdata-stng.txt` (For SeatalkNG)

#### How to look at the transmitted data:
- `telnet 127.0.0.1 4545`

