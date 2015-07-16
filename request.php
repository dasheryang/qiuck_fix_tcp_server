<?php
error_reporting(E_ALL);

$sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if( false === $sock ){
        echo "sock create error!\n";
}

$address = 'localhost';
$port = 51003;
$result = socket_connect($sock, $address, $port);
if ($result === false) {
    echo "socket_connect() failed.\nReason: " . socket_strerror(socket_last_error($sock)) . "\n";
    return;
} else {
    echo "OK.\n";
}


$json_dat = array(
        'in' => 'test in',
        'head_keys' => array(8 , 49, 56, 35 ),
        'head_vals' => array( "FIX.4.2", "GOLDMF", "KGITEST", "D" ),
        'body_keys' => array( 1, 11, 21, 38, 40, 44, 54, 55, 60 ),
	'body_vals' => array( "ACCOUNTPHP", "1234567", "1", "1000", "7", "9,54", "1", "0700", "2015-07-09T01:08:18Z" ),
	'out' => 'test out',
);

$in_str = json_encode( $json_dat );

socket_write( $sock, $in_str, strlen($in_str) );

while( $out_str = socket_read($sock, 2048) ){
        echo "revice result\n";
        echo $out_str . "\n";
        $json_data = json_decode( $out_str );
        var_dump( $json_data );


        break;
}

socket_close( $sock );

