const slideValue = document.querySelector("span");
const inputslider = document.querySelector("input");
inputslider.oninput = (() => {
    let minvalue = document.querySelector(".left").textContent;
    let value = inputslider.value;
    slideValue.textContent = value;
    slideValue.style.left = ((value - minvalue) * 5) + "%";
    slideValue.classList.add("show");
});
setInterval(() => {
    slideValue.classList.remove("show");
}, 3000);