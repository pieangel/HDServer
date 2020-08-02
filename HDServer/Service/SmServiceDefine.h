#pragma once
enum class SmProtocol {
	req_none = 0,
	// 로그인 요청
	req_login,
	// 로그인 응답
	res_login,
	// 로그아웃 요청
	req_logout,
	// 로그아웃 응답
	res_logout,
	// 실시간 시세 등록 요청
	req_register_symbol,
	// 실시간 시세 등록 응답
	res_register_symbol,
	// 실시간 시세 해제 요청
	req_unregister_symbol,
	// 실시간 시세 해제 응답
	res_unregister_symbol,
	// 주기 차트 데이터 등록 요청
	req_register_chart_cycle_data,
	// 주기 차트 데이터 등록 응답
	res_register_chart_cycle_data,
	// 신규 주문 요청
	req_order_new,
	// 신규 주문 요청 응답
	res_order_new,
	// 정정 주문 요청
	req_order_modify,
	// 정정 주문 응답
	res_order_modify,
	// 취소 주문 요청
	req_order_cancel,
	// 취소 주문 응답
	res_order_cancel,
	// 주문 접수확인 응답
	res_order_accepted,
	// 주문 체결 응답
	res_order_filled,
	// 차트데이터 요청
	req_chart_data,
	// 차트 데이터 응답
	res_chart_data,
	// 시세 데이터 요청
	req_sise_data,
	// 시세 데이터 응답
	res_sise_data,
	// 실시간 호가 전송
	res_realtime_hoga,
	// 실시간 시세 전송
	res_realtime_sise,
	// 주기 차트 데이터 전송
	res_chart_cycle_data,
	// 주기 차트 데이터 해제 요청
	req_unregister_chart_cycle_data,
	// 주기 차트 데이터 해제 응답
	res_unregister_chart_cycle_data,
	// 호가데이터 요청
	req_hoga_data,
	// 호가데이터 응답
	res_hoga_data,
	// 심볼 마스트 요청
	req_symbol_master,
	// 심볼 마스터 응답
	res_symbol_master,
	// 모든 심볼 마스터 요청
	req_symbol_master_all,
	// 모든 종목 현시세 요청
	req_sise_data_all,
	// 최근월물 모든 현시세 요청
	req_recent_sise_data_all,
	// 최근월물 실시간 시세 등록 요청
	req_register_recent_realtime_sise_all,
	// 마켓 목록 요청
	req_market_list,
	// 마켓 목록 응답
	res_market_list,
	// 카테고리별 종목 리스트 요청
	req_symbol_list_by_category,
	// 종목 잔고 정보
	res_symbol_position,
	// 주문 관련 오류
	res_order_error,
	// 포지션 목록 요청
	req_position_list,
	// 포지션 목록 응답
	res_position_list,
	// 시세 소켓 등록 요청
	req_register_sise_socket,
	// 시세 소켓 등록 응답
	res_register_sise_socket,
	// 시세 차트에서 차트 데이터를 받았음을 알리고 클라이언트에게 전송을 종용한다.
	req_chart_data_resend,
	// 메인차트에서 차트 데이터 요청이 도착함
	req_chart_data_from_main_server,
	// 심볼 시세 업데이트 요청
	req_update_quote,
	// 심볼 호가 업데이트 요청
	req_update_hoga,
	// 차트데이터가 업데이트 되었음을 알린다.
	req_update_chart_data,
	// 사용자 등록을 요청한다.
	req_register_user,
	// 사용자 등록 요청에 대한 응답
	res_register_user,
	// 사용자 등록 해제 요청
	req_unregister_user,
	// 사용자 등록 해제 요청 응답
	res_unregister_user,
	// 계좌 목록 요청
	req_account_list,
	// 계좌 목록 응답
	res_account_list,
	// 차트 데이터 일대일 요청
	req_chart_data_onebyone,
	// 차트 데이터 일대일 응답
	res_chart_data_onebyone,
	// 접수확인 주문 목록 요청
	req_accepted_order_list,
	// 접수확인 주문 목록 응답
	res_accepted_order_list,
	// 체결 주문 목록 요청
	req_filled_order_list,
	// 체결 주문 목록 응답
	res_filled_order_list,
	// 주문 목록 요청
	req_order_list,
	// 주문 목록 응답
	res_order_list,
	// 차트 데이터를 하나씩 보낸다.
	req_chart_data_resend_onebyone,
	// 주기 데이터를 하나씩 보낸다.
	req_cycle_data_resend_onebyone,
	// 주문이 청산되었음을 보낸다.
	res_order_settled,
	// 계좌 수수료 요청
	req_account_fee,
	// 계좌 수수료 응답
	res_account_fee,
	// 심볼 수수료 요청
	req_symbol_fee,
	// 심볼 수수료 응답
	res_symbol_fee,
	// 거래 발생에 따른 수수료 전송
	res_trade_fee,
	// 모의 계좌 초기화 요청
	req_reset_account,
	// 모의 계좌 초기화 응답
	res_reset_account,
	// 수익 목록 요청
	req_trade_list,
	// 수익 목록 응답
	res_trade_list,
	// 최근 심볼에 대한 호가 정보
	req_recent_hoga_data_all,
	// 최근 심볼에 대한 호가 정보
	res_recent_hoga_data_all,
	// 지표값 요청
	req_indicator,
	// 지표값 응답
	res_indicator,
	// 상관관계값 요청
	req_correlation,
	// 상관관계값 응답
	res_correlation
};