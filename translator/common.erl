% ----------------------------------------------------------------------------
% "THE BEER-WARE LICENSE" (Revision 42):
% <datafl4sh@toxicnet.eu> wrote this file. As long as you retain this notice you
% can do whatever you want with this stuff. If we meet some day, and you think
% this stuff is worth it, you can buy me a beer in return. datafl4sh.
% ----------------------------------------------------------------------------

-module(common).
-export([worker/2, tuplespace/1, run_tcp_server/3, run_server/1, update_nav_data/3, run_tuplespace/0]).
-import(jiffy, [decode/1]).

-define(TCP_OPTIONS, [binary, {packet, 0}, {active, false}, {reuseaddr, true}]).
-define(NUM_WORKERS, 20).
-define(LISTEN_PORT, 4545).


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Trivial tuplespace to hold navigation data.
%
% The data collection process -when it has data available-
% sends a message to the tuplespace to publish it.
% The server process when a client connects sends a message
% to the tuplespace to retrieve the current navigation data.

tuplespace(Tuples) ->
    receive
        {_, {insert, Name, Value}} ->
            Tpls = lists:filter( fun({N, _}) -> N /= Name end, Tuples),
            tuplespace([{Name, Value} | Tpls]);

        {From, {getall}} ->
            From ! Tuples,
            tuplespace(Tuples)
    end.

run_tuplespace() ->
    spawn(?MODULE, tuplespace, [[]]).

update_nav_data(Ts, Name, Value) ->
    Ts ! {self(), {insert, Name, Value}}.

get_nav_data(Ts) ->
    Ts ! {self(), {getall}},
    receive
        Response ->
            Response
    end.


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% TCP server.
%
% Should be self-explaining.

run_tcp_server(Ts, Num, LPort) ->
    case gen_tcp:listen(LPort, ?TCP_OPTIONS) of
        {ok, ListenSock} ->
            spawn_workers(Ts, Num, ListenSock),
            {ok, Port} = inet:port(ListenSock),
            Port;
        {error,Reason} ->
            {error,Reason}
    end.

spawn_workers(_, 0,_) ->
    ok;
spawn_workers(Ts, Num, LS) ->
    spawn(?MODULE, worker, [Ts, LS]),
    spawn_workers(Ts, Num-1, LS).

worker(Ts, LS) ->
    case gen_tcp:accept(LS) of
        {ok,S} ->
            handler(Ts, S),
            worker(Ts, LS);
        Other ->
            io:format("accept returned ~w - goodbye!~n",[Other]),
            ok
    end.

handler(Ts, S) ->
    Nd = get_nav_data(Ts),
    E = jiffy:encode({Nd}, [pretty]),
    inet:setopts(S,[{active,once}]),
    gen_tcp:send(S, E),
    gen_tcp:close(S).


run_server(Ts) ->
    run_tcp_server(Ts, ?NUM_WORKERS, ?LISTEN_PORT).



