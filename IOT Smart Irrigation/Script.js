const firebaseConfig = {
	apiKey: "YourApiKey",
	authDomain: "YourAuthDomain",
	databaseURL: "YourDatabaseURL",
	projectId: "YourProjectId",
	storageBucket: "YourStorageBucket",
	messagingSenderId: "YourMessSenderId",
	appId: "YourAppId"
  };
  firebase.initializeApp(firebaseConfig);

  var database = firebase.database();
  var dataref1 = database.ref('DHT_11/Temperatura');
  var dataref2 = database.ref('DHT_11/Vlaznost_zrala');
  var dataref3 = database.ref('Soil_moisture_sensor/Vlaznost tla');
  var Status_pumpe; 

  database.ref().on('value',function(snap)
  {
	Status_pumpe = snap.val().Status_pumpe;
	if(Status_pumpe ==1)
	{
		document.getElementById("btn1").innerText = "UGASI PUMPU";
	}else{
		document.getElementById("btn1").innerText = "UPALI PUMPU";
	}
  });

  dataref1.on('value', function(getData1){
	var t = getData1.val();
	progressBar(t,100,'.progress-circle-prog','.progress-text','progress','t');
  });

  dataref2.on('value', function(getData1){
	var h = getData1.val();
	progressBar(h,100,'.progress-circle-prog1','.progress-text1','progress1','h');
  });

  dataref3.on('value', function(getData1){
	var h = getData1.val();
	progressBar(h,100,'.progress-circle-prog2','.progress-text2','progress2', 'h');
	if(h<20)
	{
		document.getElementById("NaslovUpozorenje").innerText="UPOZORENJE";
		document.getElementById("ParagrafUpozorenje").innerText = "UPALI PUMPU ODMAH !!";
		document.getElementById("NaslovUpozorenje").style.color ="red";
		document.getElementById("ParagrafUpozorenje").style.color ="red";
	}
	else if(h>60)
	{
		document.getElementById("NaslovUpozorenje").innerText="UPOZORENJE"
		document.getElementById("ParagrafUpozorenje").innerText = "UGASI PUMPU ODMAH !!!";
		document.getElementById("NaslovUpozorenje").style.color ="red";
		document.getElementById("ParagrafUpozorenje").style.color ="red";
	}else
	{
		document.getElementById("NaslovUpozorenje").innerText="STANJE ZEMLJIŠTA UREDNO"
		document.getElementById("ParagrafUpozorenje").innerText = "NIKAKVE MJERE NIJE POTREBNO PODUZETI";
		document.getElementById("NaslovUpozorenje").style.color ="hsl(205, 86%, 56%)";
		document.getElementById("ParagrafUpozorenje").style.color ="hsl(205, 86%, 56%)";
	}
  });

  $('.btn').click(function()
  {
	var firebaseRef = firebase.database().ref().child("Status_pumpe");
	if(Status_pumpe ==1)
	{
		firebaseRef.set(0);
		Status_pumpe = 0;
	}else{
		firebaseRef.set(1);
		Status_pumpe = 1;
	}
  });
  
function progressBar(progressVal,totalPercentageVal = 100, progrCircp, progressText, progress, id) {
    var strokeVal = (4.64 * 100) /  totalPercentageVal;
	var x = document.querySelector(progrCircp);
    x.style.strokeDasharray = progressVal * (strokeVal) + ' 999';
	var el = document.querySelector(progressText); 
	var from = $(progressText).data(progress);
	$(progressText).data(progress, progressVal);
	var start = new Date().getTime();
  
	setTimeout(function() {
	    var now = (new Date().getTime()) - start;
	    var progress = now / 700;
	    el.innerHTML = Math.round(progressVal / totalPercentageVal * 100) + (id == 't'?'°C':'%');
	    if (progress < 1) setTimeout(arguments.callee, 10);
	}, 10);

};

