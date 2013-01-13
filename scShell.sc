/* an OSC shell for supercollider similar to Chulos */
~scShell = { arg shellPort = NetAddr.langPort, commandPort = 8000;
	n = NetAddr("127.0.0.1", shellPort);
	q = NetAddr("127.0.0.1", commandPort);
	~coll = List();
	OSCdef(\scShellNew, {|msg, time, addr, recvPort|
		var f; f = this.compileFile( msg[1].asString );

		if ( f != nil )
		{ var ret; ret = f.value; ~coll.add(ret);
			q.sendMsg("/sc/newID", ~coll.size ) }
		{ q.sendMsg("/sc/newID", 0) }}, '/sc/new', nil, shellPort);

	OSCdef(\scShellRem, {|msg, time, addr, recvPort|
		if ( ~coll[msg[1]] != nil )
		{ ~coll[msg[1]].stop;
			~coll[msg[1]] = nil;
			q.sendMsg("/sc/remID", msg[1]) }
		{ q.sendMsg("/sc/remID", 0) }}, '/sc/remove', nil, shellPort);

	[OSCdef(\scShellNew), OSCdef(\scShellRem)]
};

//test commands
/*
~scShell.value;
z = NetAddr("127.0.0.1", NetAddr.langPort);
z.sendMsg("/sc/new", "c:/Users/Graham/Desktop/sc_today/testPat.sc");
z.sendMsg("/sc/remove", 0);

~scShell.value( 3001, 3002 );
z = NetAddr("127.0.0.1", 3001);
z.sendMsg("/sc/new", "c:/Users/Graham/Desktop/sc_today/testPat.sc");
z.sendMsg("/sc/remove", 0);
*/

