/* an OSC shell for supercollider similar to Chulos */

~commNew = "/sc/new";
~commRemove = "/sc/remove";

~scShell = { arg shellPort = NetAddr.langPort, commandPort = 8000;
	n = NetAddr("127.0.0.1", shellPort);
	q = NetAddr("127.0.0.1", commandPort);
	~coll = List();
	OSCdef(\scShellNew, {|msg, time, addr, recvPort|
		var f; f = this.compileFile( msg[1].asString );

		if ( f != nil )
		//returning size, pretend numbers one-based
		{ var ret; ret = f.value; ~coll.add(ret);
			~coll.size.postln;
			q.sendMsg(~commNew, ~coll.size ) }
		{ q.sendMsg(~commNew, 0) }}, ~commNew, nil, shellPort);

	OSCdef(\scShellRem, {|msg, time, addr, recvPort|
		var ind; ind = msg[1]-1;
		if ( msg[1] > 0 && {~coll[ind] != nil} )
		{ var ind; ind = msg[1]-1; //actual index
			~coll[ind].stop;
			~coll[ind] = nil;
			q.sendMsg(~commRemove, msg[1]) }
		{ q.sendMsg(~commRemove, 0) }}, ~commRemove, nil, shellPort);

	[OSCdef(\scShellNew), OSCdef(\scShellRem)]
};

//test commands
/*
~scShell.value;
z = NetAddr("127.0.0.1", NetAddr.langPort);
z.sendMsg("/sc/new", "c:/Users/Graham/Desktop/sc_today/testPat.sc");
z.sendMsg("/sc/remove", 1);

~scShell.value( 3001, 3002 );
z = NetAddr("127.0.0.1", 3001);
z.sendMsg("/sc/new", "c:/Users/Graham/Desktop/sc_today/testPat.sc");
z.sendMsg("/sc/remove", 0);
*/
