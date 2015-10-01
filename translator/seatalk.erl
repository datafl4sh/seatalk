% ----------------------------------------------------------------------------
% "THE BEER-WARE LICENSE" (Revision 42):
% <datafl4sh@toxicnet.eu> wrote this file. As long as you retain this notice you
% can do whatever you want with this stuff. If we meet some day, and you think
% this stuff is worth it, you can buy me a beer in return. datafl4sh.
% ----------------------------------------------------------------------------

-module(seatalk).
-import(common, [run_server/1, run_tuplespace/0, update_nav_data/3]).
-export([start/0]).

drop(_, []) -> [];
drop(0, Xs) -> Xs;
drop(N, [_|Xs]) -> drop(N-1, Xs).

take(0, _) -> [];
take(N, [X|Xs]) -> [X|take(N-1, Xs)].

take_from_to(M, N, Xs) ->
    drop(M-1, take(N, Xs)).


st_concat_aux([]) -> [];
st_concat_aux([X|Xs]) ->
    XX = drop(1, X),
    [XX|st_concat_aux(Xs)].

st_concat(Xs) -> lists:flatten(lists:reverse(st_concat_aux(Xs))).

st_take([], _) -> [];
st_take([X|Xs], Data) ->
    drop(1, lists:nth(X, Data)) ++ st_take(Xs, Data).

st_take_raw([], _) -> [];
st_take_raw([X|Xs], Data) ->
    [list_to_integer(drop(1, lists:nth(X, Data)),16) | st_take_raw(Xs, Data)].

decode_message([]) -> {empty};

% Depth
decode_message(["100" | Data]) ->
    if 
        length(Data) /= 4 -> {error, "Unexpected data length"};
        true ->
            V = st_take([4,3], Data),
            T = list_to_integer(V, 16),
            Depth = T/10,
            %io:format("Depth: ~w~n", [Depth]),
            {ok, <<"depth">>, Depth}
    end;

% Apparent Wind Angle
decode_message(["110" | Data]) ->
    if
        length(Data) /= 3 -> {error, "Unexpected data_length"};
        true ->
            V = st_take([2,3], Data),
            T = list_to_integer(V, 16),
            AWA = T/2,
            %io:format("AWA: ~w~n", [AWA]),
            {ok, <<"awa">>, AWA}
    end;

% Apparent wind speed
decode_message(["111" | Data]) ->
    if
        length(Data) /= 3 -> {error, "Unexpected data_length"};
        true ->
            V = st_take_raw([2,3], Data),
            A = lists:nth(1, V) band 16#7f,
            B = lists:nth(2, V)/10,
            AWS = A+B,
            %io:format("AWS: ~w~n", [AWS]),
            {ok, <<"aws">>, AWS}
    end;

% Speed Through Water
decode_message(["120" | Data]) ->
    if
        length(Data) /= 3 -> {error, "Unexpected data_length"};
        true ->
            V = st_take([2,3], Data),
            T = list_to_integer(V, 16),
            STW = T/10,
            %io:format("STW: ~w~n", [STW]),
            {ok, <<"stw">>, STW}
    end;

% Water Temperaure
decode_message(["123" | Data]) ->
    if
        length(Data) /= 3 -> {error, "Unexpected data_length"};
        true ->
            V = st_take_raw([2,3], Data),
            TempC = lists:nth(1, V),
            TempF = lists:nth(2, V),
            %io:format("Temp (1): ~w C, ~w F~n", [TempC, TempF]),
            {ok, <<"tempc">>, TempC}
    end;

% Water Temperaure
decode_message(["127" | Data]) ->
    if
        length(Data) /= 3 -> {error, "Unexpected data_length"};
        true ->
            V = st_take([3,2], Data),
            T = list_to_integer(V, 16),
            Temp = (T-100)/10,
            %io:format("Temp (2): ~w~n", [Temp]),
            {ok, <<"temp">>, Temp}
    end;

decode_message([M|_]) ->
    {unknown, M}.

process_seatalk(Ts) ->
    case io:get_line("") of
        eof ->
            ok;
        Line ->
             Tokens = string:tokens(Line, " \n"),
             Tks = drop(1, Tokens),
             case decode_message(Tks) of
                {ok, Msg, Val} ->
                    update_nav_data(Ts, Msg, Val);
                {error, Reason} ->
                    io:format("Error in processing: ~w~n", [Reason]);
                {unknown, M} ->
                    %io:format("Unknown message: ~p~n", [M]);
                    ok;
                _ -> ok
             end,
             timer:sleep(500), % enable this to use with test data
             process_seatalk(Ts)
    end.

start() ->
    Ts = common:run_tuplespace(),
    run_server(Ts),
    process_seatalk(Ts).

