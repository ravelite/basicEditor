OscRecv orec;
7000 => orec.port;
orec.listen(); 

OscSend xmit;
xmit.setHost("localhost", 7001);

spork ~ waitNewShred();

fun void waitNewShred() {

    orec.event("/shred/new,s") @=> OscEvent newShred;

    while( true ) {

        newShred => now;
    
        // grab the next message from the queue. 
        while( newShred.nextMsg() != 0 )
        { 
            //TODO: fault tolerence
            //newShred.getString() => string shredFile;
            //<<< shredFile >>>;
            Machine.add( newShred.getString() ) => int shrid;
            xmit.startMsg("/shred/new,i");
            xmit.addInt( shrid );
        }
    }

}

while ( true ) {

   1::second => now;

}