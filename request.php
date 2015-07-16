<?php
error_reporting(E_ALL);

function send_order_request( $cl_order_id ){
    $sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    if( false === $sock ){
            echo "sock create error!\n";
    }

    $address = 'localhost';
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
            'head_keys' => array(8 , 49, 56, 35 ),
            'head_vals' => array( "FIX.4.2", "GOLDMF", "KGITEST", "D" ),
            'body_keys' => array( 1, 11, 21, 38, 40, 44, 54, 55, 60 ),
        'body_vals' => array( "ACCOUNTPHP", $cl_order_id, "1", "1000", "7", "9,54", "1", "0700", "2015-07-09T01:08:18Z" ),
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
}

function retrive_order_report( $cl_order_id ){
    $redis_svr = new Redis();
    $redis_svr->connect( "127.0.0.1", 6379 );

    $xml_report = $redis_svr->get( "FIX_EXE_{$cl_order_id}" );
    // var_dump( $xml_report );
    $report_arr = parse_report_field_map_from_xml( $xml_report );

}

function parse_report_field_map_from_xml( $xml_string ){
    $xml_obj = simplexml_load_string( $xml_string );

    $header_map = $body_map = $trailer_map = array();

    if( !empty( $xml_obj->header) ){
        $header_map = xml2arr( $xml_obj->header );
    }
    
    if( !empty( $xml_obj->body ) ){
        $body_map = xml2arr( $xml_obj->body );
    }
    
    if( !empty( $xml_obj->trailer ) ){
        $trailer_map = xml2arr( $xml_obj->trailer );
    }
    

    $map_pack = array(
        'header' => $header_map,
        'body' => $body_map,
        'trailer' => $trailer_map,
    );
    var_dump( $map_pack );
}


function xml2arr( $xml_obj ){
    if( empty( $xml_obj ) ){
        return array();
    }
    $map = array();
    foreach ($xml_obj->field as $value) {
        $f = intval( $value['number'] );
        $val = strval( $value );
        $map[$f] = $val;
    }
    return $map;
}


function main(){
    $cl_order_id = "1234569";
    echo "start client test... \n";
	send_order_request( $cl_order_id );

	echo "wait for response...\n";
	sleep(2);
	echo "order status info:\n";
    retrive_order_report( $cl_order_id );
    echo "end client test...\n";
}

main();



