dbname=testdb
#user=USERNAME
#pass=PASSWORD
host=localhost:27017

collections=`./mongo.exe "$host/$dbname" --quiet --eval "db.getCollectionNames().join(' ')" | tr '\r' '\n'`;

for i in $collections
do
    echo exporting collection $i
    keys=`./mongo.exe "$host/$dbname" --eval "var keys = []; for(var key in db.$i.find().sort({_id: -1}).limit(1)[0]) { keys.push(key); }; keys.join();" --quiet | tr '\r' '\n'`;
    ./mongoexport.exe --host $host -d $dbname -c $i --fields "$keys" --type=csv --out $dbname.$i.csv;
done