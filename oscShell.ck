//ChuLOS: ChucK Lightweight OSC Shell
//gkc: a provisional way to access the VM through OSC

OscRecv orec;
7000 => orec.port;
orec.listen(); 

OscSend xmit;
xmit.setHost("localhost", 7001);

spork ~ waitNewShred();
spork ~ waitRemoveShred();

fun void waitNewShred() {

    //the arguments are a path to the file,
    //and some sort of editor-specific ID for the shred
    orec.event("/shred/new,si") @=> OscEvent newShred;

    while( true ) {

        newShred => now;
    
        // grab the next message from the queue. 
        while( newShred.nextMsg() != 0 )
        { 
            //saving the string is crashing my chuck, 
            //newShred.getString() @=> string shredFile;
            
            Machine.add( newShred.getString() ) => int shrid;
            newShred.getInt() => int edShrid;
            
            //send the response, the editor ID with the shred ID
            xmit.startMsg("/shred/new,ii");
            xmit.addInt( edShrid );
            xmit.addInt( shrid );
        }
    }

}

fun void waitRemoveShred() {
    
    //the argument is the shred id
    orec.event("/shred/remove,i") @=> OscEvent removeShred;
    
    while( true ) {
        
        removeShred => now;
        
        // grab the next message from the queue. 
        while( removeShred.nextMsg() != 0 )
        {
            Machine.remove( removeShred.getInt() ) => int resp;
            
            //send the response directly
            xmit.startMsg("/shred/remove,i");
            xmit.addInt( resp );
        }
    }
    
}

while ( true ) {

   1::second => now;

}