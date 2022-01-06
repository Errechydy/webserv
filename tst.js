

let yearMonth;
const d = new Date();
yearMonth = d.getFullYear()+'-'+(d.getMonth()+1);
const thisMonthsDays = ['01','02','03','04','05','06','07','08','09','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31'];

let date = yearMonth+'-'+thisMonthsDays[j];

console.log(yearMonth);



https://www.livesoccertv.com/xschedule.php?pagetype=scompetition&pageid=6&date=2022-1-01&tab=Live

(async function(){
    console.log("Start saving fixters");
await saveFixturesToBackend.saveFixtures(0, 0); // save this month fixtures
await helpers.delay(60); // Wait 10 seconds
await saveFixturesToBackend.saveFixtures(1, 1); // save next month fixtures
}