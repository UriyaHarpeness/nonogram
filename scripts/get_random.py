import argparse
import requests
import xmltodict

SIZES = {'any': 0,
         'small': 1,
         'medium': 2,
         'large': 3,
         'huge': 4}


def get_random(size: str, nonogram_path: str, nonogram_id: int):
    specified_nonogram_id = nonogram_id is not None
    while True:
        if not specified_nonogram_id:
            nonogram_id = requests.post(url='https://webpbn.com/random.cgi',
                                        data=f'sid=&go=1&psize={SIZES[size]}&pcolor=1&pmulti=1&pguess=1').url.split(
                'id=')[1].split('&')[0]

        nonogram = requests.post(url='https://webpbn.com/export.cgi/webpbn001792.mk',
                                 data=f'go=1&sid=&id={nonogram_id}&xml_clue=on&xml_soln=on&ss_soln=on&sg_clue=on&sg_soln=on&fmt=mk').text

        if 'The mk format cannot handle puzzles with blotted clues.' not in nonogram:
            break

        if specified_nonogram_id:
            print(f'Puzzle ID {nonogram_id} is invalid, breaking.')
            return
        else:
            # The MK nonogram definition format does not support clues, retry is needed.
            print(f'Puzzle ID {nonogram_id} is invalid, retrying...')

    # Retrieving metadata.
    nonogram_metadata = \
        xmltodict.parse(requests.post(url='https://webpbn.com/XMLpuz.cgi', data=f'id={nonogram_id}').text)['puzzleset'][
            'puzzle']

    # Converting MK nonogram definition format to this implementation's format.
    nonogram = nonogram.split('\n')
    nonogram = [('' if line == '0' else line) for line in nonogram]

    y, x = nonogram[0].split(' ')
    x, y = int(x), int(y)

    nonogram = '\n'.join([f'{x} {y}'] + nonogram[y + 2:y + 2 + x] + nonogram[1:y + 1] + [''])

    with open(nonogram_path, 'w+') as nonogram_file:
        nonogram_file.write(nonogram)

    print(f'Title: {nonogram_metadata["title"]}.')
    print(f'Author: {nonogram_metadata["author"]}.')
    print(f'Description: {nonogram_metadata["description"]}.')
    print(f'ID: {nonogram_id}.')
    print(f'Size: {x} X {y}.')


def main():
    parser = argparse.ArgumentParser(description='Retrieve random nonogram.')

    parser.add_argument('-s', '--size', dest='size', choices=SIZES.keys(), required=False, help='Size of the nonogram')
    parser.add_argument('-o', '--output', dest='nonogram_path', type=str, required=True, help='Path to nonogram file')
    parser.add_argument('--id', dest='nonogram_id', type=int, required=False, help='Nonogram ID')

    args = parser.parse_args()

    if (args.nonogram_id is not None) and (args.size is not None):
        parser.error('"size" and "nonogram_id" cannot be used together')

    if args.nonogram_id:
        print(f'Retrieving nonogram {args.nonogram_id} for {args.nonogram_path}.')

    else:
        args.size = args.size or 'any'
        print(f'Retrieving nonogram in {args.size} size for {args.nonogram_path}.')

    get_random(args.size, args.nonogram_path, args.nonogram_id)


if __name__ == '__main__':
    main()
