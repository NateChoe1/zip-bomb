# Pretty large zip bombs

This repo contains some pretty large zip bombs, suitable for tearing down
[malicious](https://blog.haschek.at/2017/how-to-defend-your-website-with-zip-bombs.html)
[web](https://idiallo.com/blog/zipbomb-protection)
[scrapers](https://ache.one/notes/html_zip_bomb). These zip bombs are all
handcrafted. For automatically generated, very large recursive zip bombs, see
[IED](https://github.com/NateChoe1/ied).

## History

The first zip bomb in this repo (stored in `./primitive`) was hacked together in
a weekend as a proof of concept doubly compressed zip bomb.

A few months after handcrafting that initial payload, I wrote a program called
[IED](https://github.com/NateChoe1/ied) to generate much larger recursively
compressed payloads programatically. IED can create payloads much, [much
larger](https://natechoe.dev/blog/2025-08-04.html) than anything I could ever
create by hand.

A couple of weeks after the development of IED, I decided to come back and
create an "infinitely large" zip bomb as a
[quine](https://honno.dev/gzip-quine/). That is stored in `./quine`.

## Tools

These zip bombs are created with some small C tools that I wrote. To compile
them, just run `make`.

## Directories

| Path | Description |
| ---- | ----------- |
| `./primitive` | Basic doubly compressed zip bomb, decompresses twice from around 1 megabyte to around 1 terabyte. |
| `./quine` | Quine zip bomb, infinitely decompresses with an amortized compression ratio of 1032x per layer |

## Licensing

I'm donating all of the code and zip bombs in this repo to the public domain,
feel free to use it however you want.

Although I can't enforce it, please give me credit if you decide to use any of
these if possible.
