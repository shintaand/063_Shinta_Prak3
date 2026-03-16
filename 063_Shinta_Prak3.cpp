#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

struct servis {
    string model, merek, kendala, montir, tanggal;
    string nama_p, nomor_p;
    servis* next;
};

struct pelanggan {
    string nama, gender, alamat, nomor, tanggal;
    int umur;
    servis* riwayatservis;
    servis* antrianservis; 
    pelanggan *next, *prev;
};

struct montir {
    string nama;
    montir* next;
};

pelanggan* headpel = NULL;
pelanggan* tailpel = NULL;
servis* front = NULL;
servis* rear = NULL;
servis* donehead = NULL;
servis* cancelstack = NULL;
montir* headmontir = NULL;

string currentuser;
string currenttelp;


pelanggan* caripelanggan(string nama) {
    pelanggan* temp = headpel;
    while (temp != NULL) {
        if (temp->nama == nama) return temp;
        temp = temp->next;
    }
    return NULL;
}

pelanggan* tambahpelanggan(string nama, string tanggal = "", string nomor = "", string alamat = "", int umur = 0, string gender = "") {
    pelanggan* baru = new pelanggan;
    baru->nama = nama;
    baru->tanggal = tanggal;
    baru->nomor = nomor;
    baru->alamat = alamat;
    baru->umur = umur;
    baru->gender = gender;
    baru->riwayatservis = NULL;
    baru->next = NULL;
    baru->prev = tailpel;

    if (headpel == NULL) {
        headpel = tailpel = baru;
    } else {
        tailpel->next = baru;
        tailpel = baru;
    }
    return baru;
}

void simpanantrian() {
    ofstream file("antrian.csv");
        servis* s = front;
        while (s != NULL) {
            file << s->nama_p << "," << s->nomor_p << "," 
                 << s->model << "," << s->merek << "," 
                 << s->kendala << "," << s->montir << "," 
                 << s->tanggal << "\n";
            s = s->next;
    }
    file.close();
}


void loadingantrian() {
    ifstream file("antrian.csv");
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string nama, nomor, mod, mer, ken, mon, tanggal;

        getline(ss, nama, ',');
        getline(ss, nomor, ',');
        getline(ss, mod, ',');
        getline(ss, mer, ',');
        getline(ss, ken, ',');
        getline(ss, mon, ',');
        getline(ss, tanggal, ',');

        if (nama == "") continue;

        servis* baru = new servis;
        baru->nama_p = nama;
        baru->nomor_p = nomor;
        baru->model = mod;
        baru->merek = mer;
        baru->kendala = ken;
        baru->montir = mon;
        baru->tanggal = tanggal;
        baru->next = NULL;

        if (rear == NULL) {
            front = rear = baru;
        } else {
            rear->next = baru;
            rear = baru;
        }

        if (caripelanggan(nama) == NULL) {
            tambahpelanggan(nama, "", nomor); 
        }
    }
    file.close();
}

void simpan() {
    ofstream file("datalengkap.csv");
    pelanggan* p = headpel;
    while (p != NULL) {
        servis* s = p->riwayatservis;
        while (s != NULL) {
            file << p->nama << "," << p->nomor << "," << p->alamat << "," 
                 << p->umur << "," << p->gender << "," << s->model << "," 
                 << s->merek << "," << s->kendala << "," << s->montir << "," << s->tanggal << "\n";
            s = s->next;
        }
        p = p->next;
    }
    file.close();
}

void loading() {
    ifstream file("datalengkap.csv");
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string nama, nomor, alamat, umurStr, gender, mod, mer, ken, mon, tanggal;

        getline(ss, nama, ',');
        getline(ss, nomor, ',');
        getline(ss, alamat, ',');
        getline(ss, umurStr, ',');
        getline(ss, gender, ',');
        getline(ss, mod, ',');
        getline(ss, mer, ',');
        getline(ss, ken, ',');
        getline(ss, mon, ',');
        getline(ss, tanggal, ',');

        if (nama == "") continue;

        pelanggan* pel = caripelanggan(nama);
        if (pel == NULL) {
            pel = tambahpelanggan(nama, tanggal, nomor, alamat, stoi(umurStr), gender);
        }

        servis* baru = new servis;
        baru->model = mod;
        baru->merek = mer;
        baru->kendala = ken;
        baru->montir = mon;
        baru->tanggal = tanggal;
        baru->nama_p = nama;
        baru->nomor_p = nomor;
        baru->next = pel->riwayatservis;
        pel->riwayatservis = baru;
    }
    file.close();
}

void simpanmontir() {
    ofstream file("montir.txt");
        montir* m = headmontir;
        while (m != NULL) {
            file << m->nama << "\n";
            m = m->next;
    }
    file.close();
}

void loadingmontir() {
    while(headmontir != NULL) {
        montir* temp = headmontir;
        headmontir = headmontir->next;
        delete temp;
    }
    ifstream file("montir.txt");
    if (!file.is_open()) return;

    string nama;
    while (getline(file, nama)) {
        if (nama.empty()) continue;

        montir* baru = new montir;
        baru->nama = nama;
        baru->next = headmontir;
        headmontir = baru;
    }
    file.close();
}


void enqueue(servis* baru) {
    if (rear == NULL) {
        front = rear = baru;
    } else {
        rear->next = baru;
        rear = baru;
    }
}

servis* dequeue() {
    if (front == NULL) return NULL;

    servis* temp = front;
    front = front->next;

    if (front == NULL) {
        rear = NULL;
    }
    
    temp->next = NULL;
    return temp;
}

void add_done_service(servis* selesai){
    selesai->next = donehead;
    donehead = selesai;
}

void push_cancel_stack(servis* data){
    data->next = cancelstack;
    cancelstack = data;
}

servis* pop_cancel_stack(){
    if(cancelstack == NULL) return NULL;

    servis* temp = cancelstack;
    cancelstack = cancelstack->next;
    temp->next = NULL;

    return temp;
}

void tambahservis() {
    string nama, mod, mer, ken, mon, tanggal;
    cout << "Nama Pelanggan: "; getline(cin, nama);
    pelanggan* pel = caripelanggan(nama);
    
    if (pel == NULL) {
        string nomor, alamat; int umur; string gender;
        cout << "No Telp Pelanggan: "; getline(cin, nomor);
        cout << "Alamat: "; getline(cin, alamat);
        cout << "Umur: "; cin >> umur; cin.ignore();
        cout << "Gender: "; getline(cin, gender);
        pel = tambahpelanggan(nama, tanggal, nomor, alamat, umur, gender);
    }

    cout << "Model Mobil: "; getline(cin, mod);
    cout << "Merek Mobil: "; getline(cin, mer);
    cout << "Kendala: "; getline(cin, ken);
    cout << "Montir: "; getline(cin, mon);
    cout << "Tanggal Masuk: "; getline(cin, tanggal);

    servis* baru = new servis;
    baru->model = mod;
    baru->merek = mer;
    baru->kendala = ken;
    baru->montir = mon;
    baru->tanggal = tanggal;
    baru->nama_p = pel->nama;
    baru->nomor_p = pel->nomor;
    baru->next = NULL;

    enqueue(baru);

    simpanantrian();
    cout << "\nBerhasil masuk antrian" << endl;
    cout << "Tekan Enter untuk kembali..."; cin.get();
}


void selesaikan() {
    if (front == NULL) { cout << "Antrian kosong.\n"; return; }

    servis* target = front; 

    cout << "\n=== SELESAIKAN PEKERJAAN ===\n";
    cout << "Nama Pelanggan: " << target->nama_p;
    cout << "\nModel Mobil: " << target->model;
    cout << "\nMerek Mobil: " << target->merek;
    cout << "\nMontir: " << target->montir << endl;
    cout << "Selesaikan sekarang? (y/n): ";
    string opt; getline(cin, opt);

    if (opt == "y") {
        servis* selesai = dequeue();

        pelanggan* pel = caripelanggan(selesai->nama_p);
        if (pel) {
            selesai->next = pel->riwayatservis;
            pel->riwayatservis = selesai;
        }

        simpan(); 
        simpanantrian();
        cout << "Pekerjaan sudah selesai dan masuk ke riwayat.\n";
    }
}


void tampilsemuaservis() {
    cout << "\n=== SEMUA SERVIS ===\n";
    pelanggan* p = headpel;
    bool ada = false;
    while (p != NULL) {
        servis* s = p->riwayatservis;
        while (s != NULL) {
            cout << "-----------------------\n";
            cout << "Pelanggan: " << p->nama << "\nNo Telp: " << p->nomor << endl;
            cout << "Model Mobil: " << s->model << "\nMerek: " << s->merek << endl;
            cout << "Kendala: " << s->kendala << "\nMontir: " << s->montir << endl;
            s = s->next;
            ada = true;
        }
        p = p->next;
    }
    if(!ada) cout << "Belum ada data servis.\n";
    cout << "\nTekan Enter untuk kembali..."; cin.get();
}

void riwayatmontir() {
    cout << "\n=== RIWAYAT PEKERJAAN MONTIR ===\n";
    string listmontir[200];
    int count = 0;

    servis* s_antrian = front;
    while (s_antrian != NULL) {
        bool exist = false;
        int i = 0;
        while (i < count) {
            if (listmontir[i] == s_antrian->montir) { exist = true; break; }
            i++;
        }
        if (!exist && s_antrian->montir != "") {
            listmontir[count++] = s_antrian->montir;
        }
        s_antrian = s_antrian->next;
    }

    pelanggan* p = headpel;
    while (p != NULL) {
        servis* s_riwayat = p->riwayatservis;
        while (s_riwayat != NULL) {
            bool exist = false;
            int i = 0;
            while (i < count) {
                if (listmontir[i] == s_riwayat->montir) { exist = true; break; }
                i++;
            }
            if (!exist && s_riwayat->montir != "") {
                listmontir[count++] = s_riwayat->montir;
            }
            s_riwayat = s_riwayat->next;
        }
        p = p->next;
    }

    if (count == 0) {
        cout << "Belum ada data montir.\n";
        return;
    }

    int j = 0;
    while (j < count) {
        cout << j + 1 << ". " << listmontir[j] << endl;
        j++;
    }
    
    int pilih; cout << "Pilihan: "; cin >> pilih; cin.ignore();
    if (pilih < 1 || pilih > count) return;

    string target = listmontir[pilih - 1];
    cout << "\n=== RIWAYAT KERJA: " << target << " ===\n";
    
    pelanggan* p_temp = headpel;
    while (p_temp != NULL) {
        servis* s_temp = p_temp->riwayatservis;
        while (s_temp != NULL) {
            if (s_temp->montir == target) {
                cout << "\nNama Pelanggan: " << p_temp->nama << endl;
                cout << "Model Mobil: " << s_temp->model << endl;
                cout << "Merek Mobil: " << s_temp->merek << endl;
                cout << "Kendala: " << s_temp->kendala<< endl;
                cout << "Tanggal Masuk: " << s_temp->tanggal << endl;
                cout << "Nomor Telepon: " << s_temp->nomor_p << endl;
            }
            s_temp = s_temp->next;
        }
        p_temp = p_temp->next;
    }
    
    cout << "\nTekan Enter untuk kembali..."; 
    cin.get();
}

void menusemuapelanggan() {
    cout << "\n=== SEMUA PELANGGAN ===\n";
    pelanggan* p = headpel;
    if (p == NULL) cout << "Data pelanggan kosong.\n";
    while (p != NULL) {
        cout << "-----------------------\n";
        cout << "Nama: " << p->nama << "\nTelepon: " << p->nomor << "\nAlamat: " << p->alamat << endl;
        
        servis* s = p->riwayatservis;
        if (s != NULL) {
            while (s->next != NULL) s = s->next;
            cout << "--- Servis Terakhir ---\nMobil: " << s->model << "\nMontir: " << s->montir << endl;
        }
        p = p->next;
    }
    cout << "\nTekan Enter untuk kembali..."; 
    cin.get();
}

void menudatapelanggan() {
    if (headpel == NULL) {
        cout << "\nData pelanggan masih kosong!\n";
        cout << "Tekan Enter..."; 
        cin.get();
        return;
    }
    pelanggan* curr = headpel;
    string pilih;
    do {
        cout << "\n=== Data Pelanggan ===\n";
        cout << "Nama: " << curr->nama << "\nTelepon: " << curr->nomor << "\nUmur: " << curr->umur << endl;
        cout << "Gender: " << curr->gender << "\nAlamat: " << curr->alamat << endl;
        cout << "-----------------------\nRiwayat Servis:\n";
        servis* s = curr->riwayatservis;
        while (s != NULL) {
            cout << "- " << s->model << " | " << s->kendala << " | Montir: " << s->montir << endl;
            s = s->next;
        }
        cout << "-----------------------\n[N]ext, [P]revious, [E]xit\nPilihan: ";
        getline(cin, pilih);
        
        if ((pilih == "n" || pilih == "N") && curr->next != NULL) curr = curr->next;
        else if ((pilih == "p" || pilih == "P") && curr->prev != NULL) curr = curr->prev;
        
    } while (pilih != "e" && pilih != "E");
}


void antrianservis() {
    cout << "\n=== ANTRIAN SERVIS ===\n";
    
    servis* temp = front;
    bool ada = false;

    if (temp == NULL) {
        cout << "Belum ada antrian servis.\n";
    } else {
    while (temp != NULL) {
            cout << "-----------------------\n";
            cout << "Pelanggan: " << temp->nama_p << endl;
            cout << "Nomor Telepon: " << temp->nomor_p << endl;
            cout << "Model Mobil: " << temp->model << endl;
            cout << "Merek Mobil: " << temp->merek << endl;
            cout << "Kendala: " << temp->kendala << endl;
            cout << "Montir: " << temp->montir << endl;
            cout << "Status: Menunggu / Sedang dikerjakan" << endl;

            temp = temp->next;
            ada = true;
        }
    }
    cout << "\nTekan Enter untuk kembali..."; 
    cin.get();
}


void riwayatservisanda () {
    string nama;
    cout << "\n=== RIWAYAT SERVIS ANDA ===\n";

    pelanggan* p = caripelanggan(currentuser);
    if (p != NULL && p->riwayatservis != NULL) {
        servis* s = p->riwayatservis;
        while (s != NULL) {
            cout << "-----------------------\n";
            cout << "Pelanggan: " << p->nama << "\nNo Telp: " << p->nomor << "\nTanggal Masuk: " << p->tanggal << endl;
            cout << "Model Mobil: " << s->model << "\nMerek: " << s->merek << endl;
            cout << "Kendala: " << s->kendala << "\nMontir: " << s->montir << endl;
            s = s->next;
        }
    } else {
        cout << "Belum ada data servis.\n";
    }
    cout << "\nTekan Enter untuk kembali..."; cin.get();
    return;
}

void antrianpelanggan(string nama) {
    cout << "\n=== SEMUA SERVIS ===\n";
    
    servis* s = front;
    int posisi = 1;
    bool ada = false;

    if (s == NULL) {
        cout << "Antrian kosong.\n";
    } else {
    while (s != NULL) {
            cout << "-----------------------\n";
            cout << "Pelanggan: " << s->nama_p << endl;
            cout << "Nomor Telepon: " << s->nomor_p << endl;
            cout << "Model Mobil: " << s->model << endl;
            cout << "Merek Mobil: " << s->merek << endl;
            cout << "Kendala: " << s->kendala << endl;
            cout << "Montir: " << s->montir << endl;

            s = s->next;
        }

        s = front;
        int hitung = 1;
        while (s != NULL) {
            if (s->nama_p == nama) {
                cout << "\nServis " << s->model << " Anda berada di antrian ke-" << hitung << endl;
                ada = true;
            }
            s = s->next;
            hitung++;
        }
    }
    cout << "\nTekan Enter untuk kembali..."; 
    cin.get();
}


void tambahmontir(){
    loadingmontir();
    string nama;

    cout << "=== MONTIR BARU ===\n";
    cout << "Masukkan Nama montir baru!\n";
    cout << "Nama: ";
    getline(cin,nama);

    montir* scanner = headmontir;

    while(scanner != NULL){

        if(scanner->nama == nama){

            cout << "" << nama << " sudah terdaftar sebagai montir.\n";
            cout << "Tekan Enter untuk kembali...";
            cin.get();
            return;
        }

        scanner = scanner->next;
    }

    montir* baru = new montir();
    baru->nama = nama;
    baru->next = headmontir;
    headmontir = baru;

    cout << "" << nama << " telah ditambahkan ke daftar montir.\n";
    simpanmontir();

    cout << "Tekan Enter untuk kembali...";
    cin.get();
}

string pilihmontir(){
    loadingmontir();
    montir* scanner = headmontir;
    int i = 1;

    while(scanner != NULL){
        cout << i << ". " << scanner->nama << endl;
        scanner = scanner->next;
        i++;
    }

    int pilih;
    cout << "Pilihan: ";
    cin >> pilih;
    cin.ignore();

    scanner = headmontir;
    i = 1;

    while(scanner != NULL){

        if(i == pilih) return scanner->nama;

        scanner = scanner->next;
        i++;
    }

    return "";
}

void bookingservis() {
    string model, merek, kendala, tanggal;
    
    cout << "=== SERVIS BARU ===\n";
    cout << "Model Mobil: "; getline(cin, model);
    cout << "Merek Mobil: "; getline(cin, merek);
    cout << "Kendala: "; getline(cin, kendala);
    cout << "Tanggal Masuk Bengkel: "; getline(cin, tanggal);

    servis* temp = front;
    servis* ditemukan = NULL;
    int posisi = 1;
    int antrianKe = 0;

    while (temp != NULL) {
        if (temp->nama_p == currentuser && temp->model == model && temp->merek == merek) {
            ditemukan = temp;
            antrianKe = posisi;
            break;
        }
        temp = temp->next;
        posisi++;
    }

    if (ditemukan != NULL) {

        cout << "Mobil ini sudah di bengkel, anda ingin menambahkan kendala ini kepada servis tersebut? (y/n): ";
        string pilih;
        getline(cin, pilih);

        if (pilih == "y") {

            ditemukan->kendala = ditemukan->kendala + " dan " + kendala;
            simpanantrian();
            cout << "Kendala sudah diupdate, nomor antrian anda adalah: " << antrianKe << "\n";
            cout << "Tekan Enter untuk kembali...";
            cin.get();
            return;
        }
    }

    servis* baru = new servis();
    baru->model = model;
    baru->merek = merek;
    baru->kendala = kendala;
    baru->tanggal = tanggal;
    cout << "Pilih Montir:\n";
    baru->montir = pilihmontir();
    baru->nama_p = currentuser;
    baru->nomor_p = currenttelp;
    baru->next = NULL;

    enqueue(baru);
    simpanantrian();

    int total = 0;
    servis* hitung = front;
    while(hitung != NULL) {
        total++;
        hitung = hitung->next;
    }

    cout << "Servis sudah tercatat, nomor antrian anda adalah: " << total << "\n";
    cout << "Tekan Enter untuk kembali...";
    cin.get();
}

void batalkanservis() {
    servis* scanner = front;
    servis* prev = NULL;

    int i = 1;

    cout << "=== BATALKAN SERVIS ===\n";

    while(scanner != NULL){

        if(scanner->nama_p == currentuser){

            cout << i << ".\n";
            cout << "Model Mobil: " << scanner->model << endl;
            cout << "Merek Mobil: " << scanner->merek << endl;
            cout << "Kendala: " << scanner->kendala << endl;
            cout << "Montir: " << scanner->montir << endl;
            cout << endl;
        }

        scanner = scanner->next;
        i++;
    }

    int pilih;
    cout << "Pilih Mobil yang ingin dibatalkan: ";
    cin >> pilih;
    cin.ignore();

    scanner = front;
    prev = NULL;
    i = 1;

    while(scanner != NULL){

        if(i == pilih && scanner->nama_p == currentuser){

            if(prev == NULL)
                front = scanner->next;
            else
                prev->next = scanner->next;

            if (scanner == rear) rear = prev;

            push_cancel_stack(scanner);
            simpanantrian();

            cout << "Servis " << scanner->model << " telah dibatalkan.\n";

            break;
        }

        prev = scanner;
        scanner = scanner->next;
        i++;
    }

    cout << "Tekan Enter untuk kembali...";
    cin.get();
}

void undopembatalan() {
    servis* data = pop_cancel_stack();

    if(data == NULL){

        cout << "Tidak ada pembatalan.\n";
        cin.get();
        return;
    }

    cout << "=== BOOKING KEMBALI SERVIS ===\n";

    cout << "Model Mobil: " << data->model << endl;
    cout << "Merek Mobil: " << data->merek << endl;
    cout << "Kendala: " << data->kendala << endl;
    cout << "Montir: " << data->montir << endl;

    string pilih;

    cout << "Apakah anda ingin membooking kembali servis ini? (y/n): ";
    getline(cin,pilih);

    if(pilih == "y"){

        cout << "Apakah ingin di reschedule? input (-) jika tidak\n";
        cout << "Tanggal Lama: " << data->tanggal << endl;

        cout << "Tanggal Baru: ";
        string baru;
        getline(cin,baru);

        if(baru != "-") data->tanggal = baru;

        enqueue(data);
        simpanantrian();

        int nomor = 0;
        servis* temp = front;
        while (temp != NULL) {
            nomor++;
            if (temp == data) break;
            temp = temp->next;
        }

        cout << "Servis " << data->model << " telah dibooking kembali.\n";
    }

    cout << "Tekan Enter untuk kembali...";
    cin.get();
}


void menuadmin() {
    int pilihan;
    do {
        cout << "\n=== ADMIN UNOSERVIS ===\n";
        cout << "Pilih Menu!\n";
        cout << "\n1. Servis\n";
        cout << "2. Pelanggan Baru\n";
        cout << "3. Montir Baru\n";
        cout << "4. Keluar\n";
        cout << "Pilihan: ";
        if (!(cin >> pilihan)) { cin.clear(); cin.ignore(1000, '\n'); continue; }
        cin.ignore();

        if (pilihan == 1) {
            cout << "\n=== SUB-MENU SERVIS ===\n";
            cout << "1. Semua Servis Singkat\n";
            cout << "2. Servis Baru\n";
            cout << "3. Selesaikan Pekerjaan\n";
            cout << "4. Riwayat Kerja Montir\n";
            cout << "Pilihan: ";
            int p; cin >> p; cin.ignore();

            if (p == 1) tampilsemuaservis();
            else if (p == 2) tambahservis();
            else if (p == 3) selesaikan();
            else if (p == 4) riwayatmontir();

        } else if (pilihan == 2) {
            tambahservis();

        } else if (pilihan == 3) {
            tambahmontir();
        }
    } while (pilihan != 4);

    return;
}


int main() {
    loading();
    loadingantrian();

    string nama, no;
        cout << "\n=== SELAMAT DATANG DI UNOSERVIS ===\n";
        cout << "Nama: "; getline(cin, nama);
            if (nama == "adminakses777") {
            menuadmin();
        } else {
        cout << "Nomor Telp: "; getline(cin, no);
        currentuser = nama;
        currenttelp = no;

        pelanggan* p = caripelanggan(nama);
            int pilihan;
            string target = nama;

            do {
                cout << "\n=== WELCOME, " << target << "! ===\n";
                cout << "Pilih menu!\n";
                cout << "1. Antrian Anda\n";
                cout << "2. Booking Servis\n";
                cout << "3. Batalkan Servis\n";
                cout << "4. Undo Pembatalan\n";
                cout << "5. Riwayat Servis\n";
                cout << "6. Keluar\n";
                cout << "Pilihan: ";
                if (!(cin >> pilihan)) { cin.clear(); cin.ignore(1000, '\n'); }
                else { cin.ignore(); }

                if (pilihan == 1) {
                    antrianpelanggan(p->nama);

                } else if (pilihan == 2) {
                    bookingservis();

                } else if (pilihan == 3) {
                    batalkanservis();

                } else if (pilihan == 4) {
                    undopembatalan();

                } else if (pilihan == 5) {
                    riwayatservisanda();
                    
                }
            } while (pilihan != 6);
                return 0;
        }

}
