# README

## Περιγραφή του Προγράμματος

Το παρόν πρόγραμμα υλοποιεί αλγορίθμους για μη αμβλυγώνια τριγωνοποίηση ενός Planar Straight Line Graph (PSLG) χρησιμοποιώντας τη βιβλιοθήκη CGAL. Στόχος είναι να ελαχιστοποιηθεί ο αριθμός των αμβλυγώνιων τριγώνων μέσω εισαγωγής σημείων Steiner και εφαρμογής διαφόρων μεθόδων βελτίωσης. Ο κώδικας έχει σχεδιαστεί ώστε να υποστηρίζει διάφορες μεθόδους (όπως Local Search, Simulated Annealing, Ant Colony) και διάφορες στρατηγικές εισαγωγής σημείων (π.χ. MAX_EDGE, PERICENTER, POLYGON, PROJECTION).

Μετά από παρατηρήσεις των διδασκόντων, προχωρήσαμε στις εξής διορθώσεις:

- Βελτίωση στη σχεδίαση του κώδικα ώστε να είναι πιο δομημένος (δεν υπάρχουν πλέον όλα τα αρχεία/συναρτήσεις σε ένα αρχείο, έχουν γίνει οι σωστοί διαχωρισμοί κλάσεων και modules).
- Πληρέστερη υλοποίηση της διαδικασίας επαναντριγωνοποίησης του πολυγώνου, λαμβάνοντας υπόψη την αφαίρεση ακμών και τη σωστή διαχείριση των constraint ακμών, όπως επισημάνθηκε στις κριτικές.  
- Γενικότερη βελτίωση στη μέθοδο εύρεσης γειτόνων έτσι ώστε να μην αγνοείται ένα constraint όταν αυτό συναντάται, αλλά να αντιμετωπίζεται με πιο γενικό τρόπο.

## Περιεχόμενα Αρχείων Κώδικα

- **main.cpp**: Το κύριο αρχείο του προγράμματος, διαβάζει τα δεδομένα εισόδου, εκτελεί την τριγωνοποίηση και εισάγει σημεία Steiner βάσει επιλεγμένης μεθόδου.
- **steiner_strategies.h / steiner_strategies.cpp**: Περιλαμβάνουν υλοποιήσεις διαφόρων στρατηγικών εισαγωγής σημείων Steiner (π.χ. MAX_EDGE, PERICENTER, POLYGON, PROJECTION).
- **utils.hpp / utils.cpp**: Βοηθητικές συναρτήσεις για γεωμετρικούς υπολογισμούς, έλεγχο αμβλυγώνιων τριγώνων, υπολογισμό κέντρου, κτλ.
- **JsonLoader.h / JsonLoader.cpp**: Χειρίζονται τη φόρτωση δεδομένων εισόδου από ένα αρχείο JSON.
- **JsonExporter.h / JsonExporter.cpp**: Χειρίζονται την εξαγωγή των αποτελεσμάτων σε αρχείο JSON.
- **cgal_definitions.h**: Περιέχει δηλώσεις και ορισμούς τύπων για την CGAL (π.χ. τύποι σημείων, τριγωνοποίηση κτλ.).
- **CustomConstrainedDelaunayTriangulation_2.h**: Παρέχει μια προσαρμοσμένη υλοποίηση για constrained Delaunay τριγωνοποίηση.
- **graph_definitions.h**: Ορισμοί για το γράφημα της τριγωνοποίησης (CDT).
- **LocalSearch.h**, **SimpleTriangulationSearch.h**, **SimulatedAnnealingSearch.h**, **AntColonySearch.h**: Εναλλακτικές προσεγγίσεις ή heuristics για τη βελτίωση της τριγωνοποίησης (τοπική αναζήτηση, απλή αναζήτηση, προσομοιωμένη ανόπτηση, αποικία μυρμηγκιών).

## Οδηγίες Εκτέλεσης

Το πρόγραμμα εκτελείται με τη μορφή:

   ```bash
   ./polyg input.json output.json
   ```

- `input.json`: Αρχείο εισόδου που περιέχει τα σημεία, τους περιορισμούς και τις παραμέτρους της μεθόδου.
- `output.json`: Αρχείο εξόδου στο οποίο θα αποθηκευτεί η τελική τριγωνοποίηση και τα σημεία Steiner.

## Παράδειγμα Εκτέλεσης

```bash
./polyg ../data/input.json ../data_outputs/output.json
```

## Λειτουργία Αλγορίθμου

1. **Φόρτωση Δεδομένων**: Χρησιμοποιείται το `JsonLoader` για να διαβαστούν οι συντεταγμένες των σημείων, οι περιορισμοί και οι παράμετροι του αλγορίθμου.
2. **Αρχική Τριγωνοποίηση**: Δημιουργείται αρχική τριγωνοποίηση με χρήση `Constrained_Delaunay_triangulation_2` της CGAL.
3. **Έλεγχος Αμβλυγώνιων Τριγώνων**: Εντοπίζονται τα τρίγωνα με γωνίες > 90°.
4. **Εισαγωγή Σημείων Steiner**: Αν βρεθούν αμβλυγώνια τρίγωνα, επιλέγεται μια στρατηγική (MAX_EDGE, PERICENTER, POLYGON, PROJECTION) και εισάγεται ένα σημείο Steiner.
5. **Επανάληψη**: Η τριγωνοποίηση ενημερώνεται και η διαδικασία συνεχίζεται μέχρι να μην υπάρχουν αμβλυγώνια τρίγωνα ή μέχρι να επιτευχθεί ένας μέγιστος αριθμός επαναλήψεων.

## Περιγραφή Στρατηγικών Steiner

- **MAX_EDGE**: Εισάγει σημείο στο μέσο της μεγαλύτερης ακμής του τριγώνου.
- **PERICENTER**: Εισάγει σημείο στο περίκεντρο του τριγώνου.
- **POLYGON**: Προσπαθεί να χρησιμοποιήσει τους γείτονες του τριγώνου για να σχηματίσει ένα κυρτό πολύγωνο και εισάγει σημείο στο κέντρο βάρους.
- **PROJECTION**: Προβάλλει τον αμβλύ κορυφή στην απέναντι πλευρά του τριγώνου και εισάγει το σημείο εκεί.

# Δοκιμές και Παρατηρήσεις

## Περιγραφή Δοκιμών

### Αποτελέσματα με τη Μέθοδο Simulated Annealing (SA)

**Παράδειγμα 1** 

- Initial obtuse triangles: 4
- Total obtuse triangles  : 2  
- Total steiner points    : 13  
- Energy - Initial        : 20  
- Energy - Final          : 12.6  
- Alpha                   : 5  
- Beta                    : 0.2  
*Αποθήκευση σε:* `../data_outputs/output_3_sa.json`

**Παράδειγμα 2**  

- Initial obtuse triangles: 2
- Total obtuse triangles  : 2  
- Total steiner points    : 10  
- Energy - Initial        : 10  
- Energy - Final          : 12  
- Alpha                   : 5  
- Beta                    : 0.2  
*Αποθήκευση σε:* `../data_outputs/output_2_sa.json`

**Παράδειγμα 3**  

- Initial obtuse triangles: 10  
- Total obtuse triangles  : 17  
- Total steiner points    : 47  
- Energy - Initial        : 50  
- Energy - Final          : 94.4  
- Alpha                   : 5  
- Beta                    : 0.2  
*Αποθήκευση σε:* `../data_outputs/output_4_sa.json`

**Παράδειγμα 4**  

- Initial obtuse triangles: 10  
- Total obtuse triangles  : 20  
- Total steiner points    : 60  
- Energy - Initial        : 50  
- Energy - Final          : 112  
- Alpha                   : 5  
- Beta                    : 0.2  
*Αποθήκευση σε:* `../data_outputs/output_5_sa.json`

**Παράδειγμα 5**  

- Initial obtuse triangles: 8  
- Total obtuse triangles  : 16  
- Total steiner points    : 55  
- Energy - Initial        : 40  
- Energy - Final          : 91  
- Alpha                   : 5  
- Beta                    : 0.2  
*Αποθήκευση σε:* `../data_outputs/output_6_sa.json`

**Παράδειγμα 6**  

- Initial obtuse triangles: 6  
- Total obtuse triangles  : 10  
- Total steiner points    : 52  
- Energy - Initial        : 30  
- Energy - Final          : 60.4  
- Alpha                   : 5  
- Beta                    : 0.2  
*Αποθήκευση σε:* `../data_outputs/output_7_sa.json`

### Αποτελέσματα με Μέθοδο Τοπικής Αναζήτησης (Local)

**Παράδειγμα 1**  

- Initial obtuse triangles  : 4  
- Total obtuse triangles    : 1  
- Total steiner points      : 50  
*Αποθήκευση σε:* `../data_outputs/output_3_local.json`

**Παράδειγμα 2**  

- Initial obtuse triangles  : 2  
- Total obtuse triangles    : 2  
- Total steiner points      : 3  
*Αποθήκευση σε:* `../data_outputs/output_2_local.json`

**Παράδειγμα 3** 

- Initial obtuse triangles  : 10  
- Total obtuse triangles    : 10  
- Total steiner points      : 1  
*Αποθήκευση σε:* `../data_outputs/output_4_local.json`

**Παράδειγμα 4**  

- Initial obtuse triangles  : 10  
- Total obtuse triangles    : 10  
- Total steiner points      : 1  
*Αποθήκευση σε:* `../data_outputs/output_5_local.json`

**Παράδειγμα 5**  

- Initial obtuse triangles  : 8  
- Total obtuse triangles    : 7  
- Total steiner points      : 10  
*Αποθήκευση σε:* `../data_outputs/output_6_local.json`

**Παράδειγμα 6**  

- Initial obtuse triangles  : 6  
- Total obtuse triangles    : 5  
- Total steiner points      : 10  
*Αποθήκευση σε:* `../data_outputs/output_7_local.json`

### Αποτελέσματα με Μέθοδο Αποικίας Μυρμηγκιών (Ant Colony)

**Παράδειγμα 1**  

- Initial obtuse triangles: 8  
- Total obtuse triangles  : 6  
- Total steiner points    : 3  
- Energy - Initial        : 40  
- Energy - Final          : 30.6  
- Alpha                   : 5  
- Beta                    : 0.2  
*Αποθήκευση σε:* `../data_outputs/output_6_ant.json`

**Παράδειγμα 2**  

- Initial obtuse triangles: 6  
- Total obtuse triangles  : 2  
- Total steiner points    : 2  
- Energy - Initial        : 30  
- Energy - Final          : 10.4  
- Alpha                   : 5  
- Beta                    : 0.2  
*Αποθήκευση σε:* `../data_outputs/output_7_ant.json`

## Παρατηρήσεις και Συμπεράσματα

- Σε απλά ή καλά διατεταγμένα σύνολα σημείων, το πρόγραμμα μπορεί να επιτύχει τριγωνοποίηση χωρίς αμβλυγώνια τρίγωνα ή με πολύ λίγα.
- Σε πιο σύνθετες περιπτώσεις, η εισαγωγή σημείων Steiner μπορεί να αυξήσει τον αριθμό των τριγώνων ή να μην οδηγήσει σε βελτίωση.
- Η επιλογή στρατηγικής επηρεάζει σημαντικά το αποτέλεσμα.

## Συμπληρωματικά Στοιχεία

Το πρόγραμμα έχει δοκιμαστεί σε διάφορα σύνολα σημείων με διαφορετικούς περιορισμούς, εφαρμόζοντας διάφορες μεθόδους (local, sa, ant) και καταγράφοντας τον αριθμό των αμβλυγώνιων τριγώνων πριν και μετά την εισαγωγή σημείων Steiner. Τα συμπεράσματα ποικίλουν ανάλογα με τη γεωμετρία και τους περιορισμούς του προβλήματος.

## Πληροφορίες Συντελεστών

- Κυριάκος Λάμπρος Κιουράνας (ΑΜ: 1115201900238)
- Σοφία Δέσποινα Ζήκου (ΑΜ: 1115201900276)
  