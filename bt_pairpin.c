/* Request authentication */

static void cmd_auth(int dev_id, int argc, char **argv)
{
        struct hci_conn_info_req *cr;
        bdaddr_t bdaddr;
        int opt, dd;

        for_each_opt(opt, auth_options, NULL) {
            switch (opt) {
            default:
                printf("%s", auth_help);
                return;
            }
        }
        helper_arg(1, 1, &argc, &argv, auth_help);

        str2ba(argv[0], &bdaddr);

        if (dev_id < 0) {
            dev_id = hci_for_each_dev(HCI_UP, find_conn, (long) &bdaddr);
            if (dev_id < 0) {
                fprintf(stderr, "Not connected.\n");
                exit(1);
            }
        }

        dd = hci_open_dev(dev_id);
        if (dd < 0) {
            perror("HCI device open failed");
            exit(1);
        }

        cr = malloc(sizeof(*cr) + sizeof(struct hci_conn_info));
        if (!cr) {
            perror("Can't allocate memory");
            exit(1);
        }

        bacpy(&cr->bdaddr, &bdaddr);
        cr->type = ACL_LINK;
        if (ioctl(dd, HCIGETCONNINFO, (unsigned long) cr) < 0) {
            perror("Get connection info failed");
            exit(1);
        }

        if (hci_authenticate_link(dd, htobs(cr->conn_info->handle), 25000) < 0) {
            perror("HCI authentication request failed");
            exit(1);
        }

        free(cr);

        hci_close_dev(dd);
}
/* Activate encryption */

static void cmd_enc(int dev_id, int argc, char **argv)
{
    struct hci_conn_info_req *cr;
    bdaddr_t bdaddr;
    uint8_t encrypt;
    int opt, dd;

    for_each_opt(opt, enc_options, NULL) {
        switch (opt) {
        default:
            printf("%s", enc_help);
            return;
        }
    }
    helper_arg(1, 2, &argc, &argv, enc_help);

    str2ba(argv[0], &bdaddr);

    if (dev_id < 0) {
        dev_id = hci_for_each_dev(HCI_UP, find_conn, (long) &bdaddr);
        if (dev_id < 0) {
            fprintf(stderr, "Not connected.\n");
            exit(1);
        }
    }

    dd = hci_open_dev(dev_id);
    if (dd < 0) {
        perror("HCI device open failed");
        exit(1);
    }

    cr = malloc(sizeof(*cr) + sizeof(struct hci_conn_info));
    if (!cr) {
        perror("Can't allocate memory");
        exit(1);
    }

    bacpy(&cr->bdaddr, &bdaddr);
    cr->type = ACL_LINK;
    if (ioctl(dd, HCIGETCONNINFO, (unsigned long) cr) < 0) {
        perror("Get connection info failed");
        exit(1);
    }

    encrypt = (argc > 1) ? atoi(argv[1]) : 1;

    if (hci_encrypt_link(dd, htobs(cr->conn_info->handle), encrypt, 25000) < 0) {
        perror("HCI set encryption request failed");
        exit(1);
    }

    free(cr);

    hci_close_dev(dd);
}

/* dbus command for pairing
 *  dbus-send --system --print-reply --dest=org.bluez /org/bluez/1301/hci0 org.bluez.Adapter.CreatePairedDevice string:"XX:XX:XX:XX:XX:XX" objpath:/org/bluez/agent_1317 string:"NoInputNoOutput"*/


