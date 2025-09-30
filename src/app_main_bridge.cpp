void setup();
void loop();

extern "C" void app_main()
{
    setup();
    while (true)
    {
        loop();
    }
}
