const express = require('express');
const app = express();

app.set('view engine', 'ejs');

const path = require('path');
const publicPath = path.join(__dirname, 'public');
app.use(express.static(publicPath));

const viewsPath = path.join(__dirname, '/views');
app.set('views', viewsPath);

app.use(express.urlencoded({ extended: true }));

// render register UI when requesting the server home page
app.get('/', (req, res) => {
    res.render('register');
});

// send a text message with the phone number passed in
// by the request url.
// Ex. localhost:3000/sms/604XXXXXXX should text 604XXXXXXX
app.post('/sms/:phone', (req, res) => {
    const { phone } = req.params;
    if (phone == null || parseInt(phone) === NaN) {
        console.log("Error, invalid phone number");
    }
});

let formData;
app.post('/', (req, res) => {
    formData = req.body;
    res.render('register');
});

const fileSystem = require('fs');
app.get('/data', (req, res) => {
    fileSystem.writeFile('../formData.json', JSON.stringify(formData), error => {
        if (error) {
            throw error;
        }
    });
    res.send('data has been uploaded to formData.json');
});

const port = 3000;
app.listen(port, () => {
    console.log('app listening on port', port);
});