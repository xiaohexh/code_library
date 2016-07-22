enum RequestType {
	MARKET_END = 1,
	BIZ_END = 2
}

struct OrderInfo {
	1: string id,
	2: i32 flag,
	3: double fee,
	4: string key,
	5: RequestType type
}

service VOSServer {
	oneway void SendOrderInfo(1: OrderInfo oi),
}
