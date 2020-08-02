#pragma once
enum class SmProtocol {
	req_none = 0,
	// �α��� ��û
	req_login,
	// �α��� ����
	res_login,
	// �α׾ƿ� ��û
	req_logout,
	// �α׾ƿ� ����
	res_logout,
	// �ǽð� �ü� ��� ��û
	req_register_symbol,
	// �ǽð� �ü� ��� ����
	res_register_symbol,
	// �ǽð� �ü� ���� ��û
	req_unregister_symbol,
	// �ǽð� �ü� ���� ����
	res_unregister_symbol,
	// �ֱ� ��Ʈ ������ ��� ��û
	req_register_chart_cycle_data,
	// �ֱ� ��Ʈ ������ ��� ����
	res_register_chart_cycle_data,
	// �ű� �ֹ� ��û
	req_order_new,
	// �ű� �ֹ� ��û ����
	res_order_new,
	// ���� �ֹ� ��û
	req_order_modify,
	// ���� �ֹ� ����
	res_order_modify,
	// ��� �ֹ� ��û
	req_order_cancel,
	// ��� �ֹ� ����
	res_order_cancel,
	// �ֹ� ����Ȯ�� ����
	res_order_accepted,
	// �ֹ� ü�� ����
	res_order_filled,
	// ��Ʈ������ ��û
	req_chart_data,
	// ��Ʈ ������ ����
	res_chart_data,
	// �ü� ������ ��û
	req_sise_data,
	// �ü� ������ ����
	res_sise_data,
	// �ǽð� ȣ�� ����
	res_realtime_hoga,
	// �ǽð� �ü� ����
	res_realtime_sise,
	// �ֱ� ��Ʈ ������ ����
	res_chart_cycle_data,
	// �ֱ� ��Ʈ ������ ���� ��û
	req_unregister_chart_cycle_data,
	// �ֱ� ��Ʈ ������ ���� ����
	res_unregister_chart_cycle_data,
	// ȣ�������� ��û
	req_hoga_data,
	// ȣ�������� ����
	res_hoga_data,
	// �ɺ� ����Ʈ ��û
	req_symbol_master,
	// �ɺ� ������ ����
	res_symbol_master,
	// ��� �ɺ� ������ ��û
	req_symbol_master_all,
	// ��� ���� ���ü� ��û
	req_sise_data_all,
	// �ֱٿ��� ��� ���ü� ��û
	req_recent_sise_data_all,
	// �ֱٿ��� �ǽð� �ü� ��� ��û
	req_register_recent_realtime_sise_all,
	// ���� ��� ��û
	req_market_list,
	// ���� ��� ����
	res_market_list,
	// ī�װ��� ���� ����Ʈ ��û
	req_symbol_list_by_category,
	// ���� �ܰ� ����
	res_symbol_position,
	// �ֹ� ���� ����
	res_order_error,
	// ������ ��� ��û
	req_position_list,
	// ������ ��� ����
	res_position_list,
	// �ü� ���� ��� ��û
	req_register_sise_socket,
	// �ü� ���� ��� ����
	res_register_sise_socket,
	// �ü� ��Ʈ���� ��Ʈ �����͸� �޾����� �˸��� Ŭ���̾�Ʈ���� ������ �����Ѵ�.
	req_chart_data_resend,
	// ������Ʈ���� ��Ʈ ������ ��û�� ������
	req_chart_data_from_main_server,
	// �ɺ� �ü� ������Ʈ ��û
	req_update_quote,
	// �ɺ� ȣ�� ������Ʈ ��û
	req_update_hoga,
	// ��Ʈ�����Ͱ� ������Ʈ �Ǿ����� �˸���.
	req_update_chart_data,
	// ����� ����� ��û�Ѵ�.
	req_register_user,
	// ����� ��� ��û�� ���� ����
	res_register_user,
	// ����� ��� ���� ��û
	req_unregister_user,
	// ����� ��� ���� ��û ����
	res_unregister_user,
	// ���� ��� ��û
	req_account_list,
	// ���� ��� ����
	res_account_list,
	// ��Ʈ ������ �ϴ��� ��û
	req_chart_data_onebyone,
	// ��Ʈ ������ �ϴ��� ����
	res_chart_data_onebyone,
	// ����Ȯ�� �ֹ� ��� ��û
	req_accepted_order_list,
	// ����Ȯ�� �ֹ� ��� ����
	res_accepted_order_list,
	// ü�� �ֹ� ��� ��û
	req_filled_order_list,
	// ü�� �ֹ� ��� ����
	res_filled_order_list,
	// �ֹ� ��� ��û
	req_order_list,
	// �ֹ� ��� ����
	res_order_list,
	// ��Ʈ �����͸� �ϳ��� ������.
	req_chart_data_resend_onebyone,
	// �ֱ� �����͸� �ϳ��� ������.
	req_cycle_data_resend_onebyone,
	// �ֹ��� û��Ǿ����� ������.
	res_order_settled,
	// ���� ������ ��û
	req_account_fee,
	// ���� ������ ����
	res_account_fee,
	// �ɺ� ������ ��û
	req_symbol_fee,
	// �ɺ� ������ ����
	res_symbol_fee,
	// �ŷ� �߻��� ���� ������ ����
	res_trade_fee,
	// ���� ���� �ʱ�ȭ ��û
	req_reset_account,
	// ���� ���� �ʱ�ȭ ����
	res_reset_account,
	// ���� ��� ��û
	req_trade_list,
	// ���� ��� ����
	res_trade_list,
	// �ֱ� �ɺ��� ���� ȣ�� ����
	req_recent_hoga_data_all,
	// �ֱ� �ɺ��� ���� ȣ�� ����
	res_recent_hoga_data_all,
	// ��ǥ�� ��û
	req_indicator,
	// ��ǥ�� ����
	res_indicator,
	// ������谪 ��û
	req_correlation,
	// ������谪 ����
	res_correlation
};