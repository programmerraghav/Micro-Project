// Import the functions you need from the SDKs you need
import { initializeApp } from "https://www.gstatic.com/firebasejs/11.6.1/firebase-app.js";
import { getDatabase, ref, set, onValue } from "https://www.gstatic.com/firebasejs/11.6.1/firebase-database.js";

const firebaseConfig = {
    apiKey: "AIzaSyA9l-wtUvAVE4HSJ8WF_KQ0s2fAN4S9m9c",
    authDomain: "smart-cup-coaster.firebaseapp.com",
    databaseURL: "https://smart-cup-coaster-default-rtdb.firebaseio.com",
    projectId: "smart-cup-coaster",
    storageBucket: "smart-cup-coaster.firebasestorage.app",
    messagingSenderId: "420232722948",
    appId: "1:420232722948:web:ca4d1fce733c3a965ed351",
    measurementId: "G-3PX4QQD1C8"
};

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);
let inputslider = parseInt(document.getElementById("Slider").value);
let isHot = true;
let currentTemperature;

document.getElementById("Setbutton").addEventListener('click', function () {
    inputslider = parseInt(document.getElementById("Slider").value);
    document.getElementById("Limit").innerHTML = `Current Limited Temperature: ${inputslider}&deg;C`;
    writeTemperature();
})

document.getElementById("check").addEventListener("click", function () {
    isHot = !isHot;
    if (!isHot) {
        document.querySelector("#Slider").min = 5;
        document.querySelector("#Slider").max = 25;
        document.querySelector("#Slider").value = 15;
        document.querySelector(".left").innerHTML = 5;
        document.querySelector(".right").innerHTML = 25;
        inputslider = 15;
    } else {
        document.querySelector("#Slider").min = 45;
        document.querySelector("#Slider").max = 65;
        document.querySelector("#Slider").value = 55;
        document.querySelector(".left").innerHTML = 45;
        document.querySelector(".right").innerHTML = 65;
        inputslider = 55;
    }
    document.getElementById("Limit").innerHTML = `Current Limited Temperature: ${inputslider}&deg;C`
    writeTemperature();
})

function writeTemperature() {
    set(ref(db, 'user1'), {
        temperature: inputslider,
        isHot: isHot
    });
};

const userref = ref(db, 'user2/currentTemperature')
setInterval(() => {
    onValue(userref, (snapshot) => {
        document.querySelector(".temperature").innerHTML = `${snapshot.val()}&deg;C`;
        currentTemperature = snapshot.val();
    })
    if (isHot) {
        document.querySelector(".notificationbox").style.display="normal"
        if (currentTemperature < inputslider) {
            document.querySelector(".notificationbox").innerHTML = `<div class="notification">
            <div class="header2">
                <div class="title">
                    Heating
                </div>
                <button id="close">&#10006;</button>
            </div>
            <div class="body">
                Warming up your cup - stay cozy!
            </div>
        </div>`;
        document.querySelector(".status").innerHTML = "Warming";
        } else {
            document.querySelector(".notificationbox").innerHTML = `<div class="notification">
        <div class="header2">
                <div class="title">
                Heating
                </div>
                <button id="close">&#10006;</button>
                </div>
                <div class="body">
                Your cup is hot enough - enjoy!
                </div>
                </div>`;
                document.querySelector(".status").innerHTML = "Warmed";
        }
        document.querySelector(".status").style.color = "rgba(255, 0, 0, 0.76)";
    } else {
        if (currentTemperature > inputslider) {
            document.querySelector(".notificationbox").innerHTML = `<div class="notification">
                <div class="header2">
                <div class="title">
                Cooling
                </div>
                <button id="close">&#10006;</button>
                </div>
                <div class="body">
                Almost there! Your beverage is getting ready.
                </div>
                </div>`;

            document.querySelector(".status").innerHTML = "Cozing";
        } else {
            document.querySelector(".notificationbox").innerHTML = `<div class="notification">
            <div class="header2">
            <div class="title">
            Cooling
            </div>
                <button id="close">&#10006;</button>
                </div>
                <div class="body">
                Your cup is cool enough - enjoy!
                </div>
                </div>`;

            document.querySelector(".status").innerHTML = "Cozed";
        }
        document.querySelector(".status").style.color = "rgba(0, 0, 255, 0.76)";
    }

    document.querySelector("#close").addEventListener("click", function () {
        document.querySelector(".notificationbox").innerHTML = "";
    })
}, 1000)