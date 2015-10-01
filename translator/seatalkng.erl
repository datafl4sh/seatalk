% ----------------------------------------------------------------------------
% "THE BEER-WARE LICENSE" (Revision 42):
% <datafl4sh@toxicnet.eu> wrote this file. As long as you retain this notice you
% can do whatever you want with this stuff. If we meet some day, and you think
% this stuff is worth it, you can buy me a beer in return. datafl4sh.
% ----------------------------------------------------------------------------

-module(seatalkng).
-export([start/0]).
-import(jiffy, [decode/1]).
-import(common, [run_server/1, run_tuplespace/0, update_nav_data/3]).



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Actisense JSON data input

decode_tuple({<<"Wind Speed">>, X}, Ts) ->
    common:update_nav_data(Ts, <<"aws">>, X);

decode_tuple({<<"Wind Angle">>, X}, Ts) ->
    common:update_nav_data(Ts, <<"awa">>, X);
    
decode_tuple({<<"COG">>, X}, Ts) ->
    common:update_nav_data(Ts, <<"cog">>, X);

decode_tuple({<<"SOG">>, X}, Ts) ->
    common:update_nav_data(Ts, <<"sog">>, X);

decode_tuple({<<"Latitude">>, X}, Ts) ->
    common:update_nav_data(Ts, <<"lat">>, X);

decode_tuple({<<"Longitude">>, X}, Ts) ->
    common:update_nav_data(Ts, <<"lon">>, X);

decode_tuple({<<"Date">>, X}, Ts) ->
    Y = binary:bin_to_list(X),
    common:update_nav_data(Ts, <<"date">>, Y);

decode_tuple({<<"Time">>, X}, Ts) ->
    Y = binary:bin_to_list(X),
    common:update_nav_data(Ts, <<"time">>, Y);

decode_tuple(_, _) ->
    ok.


process_tuples([], _) -> ok;
process_tuples([X|Xs], Ts) ->
    decode_tuple(X, Ts),
    process_tuples(Xs, Ts).

process_input(Ts) ->
    case io:get_line("") of
        eof ->
            ok;
        Line ->
            Dect = jiffy:decode(Line),
            List = element(1, Dect),
            Dec = lists:last(List),
            TT = element(2, Dec),
            TU = element(1, TT),
            process_tuples(TU, Ts),
            timer:sleep(500), % enable this to use with test data
            process_input(Ts)
    end.

start() ->
    Ts = common:run_tuplespace(),
    common:run_server(Ts),
    process_input(Ts).

