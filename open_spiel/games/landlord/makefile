all:landlord_card_test landlord_deck_test landlord_parser_test landlord_action_test util_test 
clean:
	rm -f *_test
landlord_card_test:landlord_card_test.cc
	g++ -g -o landlord_card_test landlord_card_test.cc
landlord_deck_test:landlord_deck_test.cc landlord_deck.* landlord_card.*
	g++ -g -o landlord_deck_test landlord_deck_test.cc landlord_deck.cc
landlord_parser_test:landlord_parser_test.cc landlord_parser.* landlord_card.* util.* landlord_move.*
	g++ -g -o landlord_parser_test landlord_parser_test.cc landlord_parser.cc landlord_move.cc util.cc
landlord_action_test:landlord_action_test.cc landlord_action.* landlord_parser.* landlord_card.* util.* landlord_move.*
	g++ -g -o landlord_action_test landlord_action_test.cc landlord_action.cc landlord_parser.cc landlord_move.cc util.cc
util_test:util_test.cc util.*
	g++ -g -o util_test util_test.cc util.cc
