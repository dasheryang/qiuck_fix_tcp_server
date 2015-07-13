<?php
error_reporting(E_ALL);

$sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if( false === $sock ){
        echo "sock create error!\n";
}

$address = '192.168.0.21';
$port = 51003;
$result = socket_connect($sock, $address, $port);
if ($result === false) {
    echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($sock)) . "\n";
    return;
} else {
    echo "OK.\n";
}


$json_dat = array(
        'in' => 'test in',
        'k_set' => array(93, 94, 95),
        'v_set' => array('v93', 'v94', 'v95'),
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

