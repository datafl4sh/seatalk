//variabile che contiene l'indirizzo del server
//const myurl = "server/example.json";
const myurl = "server/server.php";
var awsV = 0, sowV = 0, awaV = 0, mhV = 0, twsV = 0, twaV = 0, sogpV = 0, togpV = 0, cogV = 0, sogV = 0, hisV, lat = 45.67184, lon = 12.23843;

//creo la variabile di sessione per memorizzare lo storico del vento
if(!sessionStorage.hisV){
  hisV = new Array();
  sessionStorage.setItem("hisV", hisV);
}
//funzione di aggiornamento dati
function updatingData(){
  $.ajax({
    url: myurl,
    type:"GET",
    dataType: "json",
    crossDomain: true,
    ContentType: "application/json",
    
    //funzione di successo di connessione
    success: function(data, textStatus, jqXHR){
      
      //salvo i dati ricevuti nelle rispettive variabili
      var json = JSON.parse(JSON.stringify(data));
          awsV = json.aws || awsV;
          sowV = json.sow || sowV;
          awaV = json.awa || awaV;
          mhV = json.mh || mhV;
          twsV = json.tws || twsV;
          twaV = json.twa || twaV;
          sogV = json.sog || sogV;
          cogV = json.cog || cogV;
          sogpV = json.sogp || sogpV;
          togpV = json.togp || togpV;
	  lat = json.lat || lat;
          lon = json.lon || lon;

          //salvo i dati dello storico in un vettore di 180 elementi
          hisV = sessionStorage.getItem("hisV").split(",");
          if(hisV.length>=180)
            hisV.shift();
          hisV.push(awaV);
          sessionStorage.removeItem("hisV");
          sessionStorage.setItem("hisV", hisV);
          
          //aggiorno le informazioni da visualizzare
          refreshInformations();
      console.log("connection success : " + textStatus);
    },
    
    //funzione di errore di connessione
    error :function(jqXHR, textStatus, errorThrown){
      console.log("error : " + jqXHR + " : " + textStatus + " : " + errorThrown);
    },
    
    //funzione di completamento di connessione
    complete :function(qXHR, textStatus){
      console.log("complete : " + textStatus);
    }
  });
  
  //definisco il tempo di aggiornamento dei dati
  setTimeout(updatingData, 1000);
}